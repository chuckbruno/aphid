/*
 * reference
 * http://www.cs.cornell.edu/courses/cs5643/2013sp/a3Rigidbody/
 * http://www-cs-students.stanford.edu/~eparker/files/PhysicsEngine/
 * http://www.rowlhouse.co.uk/jiggle/index.html
 * http://www.richardtonge.com/papers/Tonge-2012-MassSplittingForJitterFreeParallelRigidBodySimulation-preprint.pdf
 */

#include "contactSolverCommon.cuh"
#include <CudaBase.h>

#define DEFORMABILITY 0.0134f
#define ENABLE_DEFORMABILITY 0
#define VERYLARGE_INT 16777215 // 1<<24 - 1
#define VERYLARGE_INT_M1 16777214 
#define VERYVERYLARGE_INT 1073741823 // 1<<30 - 1

__constant__ float CSpeedLimit;

inline __device__ void computeBodyAngularVelocity(float3 & angularVel,
                                                  float3 averageLinearVel,
                                                  float3 * position,
                                                  float3 * velocity,
                                                  uint4 ind)
{
    float3 center;
	float3_average4(center, position, ind);
	
	float3 omega[4];
// omega = r cross v
// v = omega cross r
    omega[0] = float3_cross(float3_difference(position[ind.x], center), float3_difference(velocity[ind.x], averageLinearVel));
    omega[1] = float3_cross(float3_difference(position[ind.y], center), float3_difference(velocity[ind.y], averageLinearVel));
    omega[2] = float3_cross(float3_difference(position[ind.z], center), float3_difference(velocity[ind.z], averageLinearVel));
    omega[3] = float3_cross(float3_difference(position[ind.w], center), float3_difference(velocity[ind.w], averageLinearVel));
    
	float3_average4_direct(angularVel, omega);
}

inline __device__ void computeBodyVelocities1(uint * pointStarts, 
                                                uint * indexStarts, 
                                                uint4 * indices, 
                                                uint ind,
                                                float3 * position,
                                                float3 * velocity, 
                                                float3 & linearVelocity, 
                                                float3 & angularVelocity)
{
    const uint4 ia = computePointIndex(pointStarts, indexStarts, indices, ind);
	
	float3_average4(linearVelocity, velocity, ia);
	computeBodyAngularVelocity(angularVelocity, linearVelocity, position, velocity, ia);
}

inline __device__ void computeBodyCenter(float3 & center, 
                                uint iBody,
                                float3 * position,
                                uint4 * indices,
                                uint * pointStarts,
                                uint * indexStarts)
{
    const uint4 iVert = computePointIndex(pointStarts, indexStarts, indices, iBody);
	float3_average4(center, position, iVert);
}

inline __device__ uint getBodyCountAt(uint ind, uint * count)
{
    uint cur = ind;
    for(;;) {
        if(count[cur] > 0) return count[cur];
        cur--;
    }
}

inline __device__ float computeRelativeVelocity(float3 nA,
                            float3 nB,
                            float3 linearVelocityA, 
                            float3 linearVelocityB,
                            float3 torqueA,
                            float3 torqueB,
                            float3 angularVelocityA, 
                            float3 angularVelocityB)
{
    return float3_dot(linearVelocityA, nA) +
            float3_dot(linearVelocityB, nB);// +
            // float3_dot(torqueA, angularVelocityA) +
            // float3_dot(torqueB, angularVelocityB);
}

inline __device__ void deformMotion(float3 & dst,
                                    float3 r, 
                                    float3 n,
                                    float3 omega)
{
// v = omega X r 
    dst = float3_cross(omega, r);
    float l = float3_length2(dst);
    float lr = float3_length(r);
// limit size of rotation
    if(l> lr * .59f) l = lr * .59f;
    if(l>1e-2) dst = float3_normalize(dst);
    dst = scale_float3_by(dst, l);
    dst = float3_add(dst, scale_float3_by(n, lr));
    dst = scale_float3_by(dst, DEFORMABILITY);
}

inline __device__ void addDeltaVelocity(float3 & dst, 
        float3 deltaLinearVelocity,
        float3 deltaAngularVelocity,
        float3 normal, 
        float3 r,
        BarycentricCoordinate * coord)
{
    dst = float3_add(dst, deltaLinearVelocity);
#if ENABLE_DEFORMABILITY
    float3 vRot;
    deformMotion(vRot, r, normal, deltaAngularVelocity);
    
// distribure by weight to vex, then sum by weight from vex
    float wei = coord->x * coord->x;
    wei = wei > 1.f ? 1.f : wei;
    dst = float3_add(dst, scale_float3_by(vRot, wei));
    
    wei = coord->y * coord->y;
    wei = wei > 1.f ? 1.f : wei;
    dst = float3_add(dst, scale_float3_by(vRot, wei));
    
    wei = coord->z * coord->z;
    wei = wei > 1.f ? 1.f : wei;
    dst = float3_add(dst, scale_float3_by(vRot, wei));
    
    wei = coord->w * coord->w;
    wei = wei > 1.f ? 1.f : wei;
    dst = float3_add(dst, scale_float3_by(vRot, wei));
#endif
}

inline __device__ float getPntTetWeight(uint pnt, 
                            uint4 tet, 
                            BarycentricCoordinate coord)
{
    if(pnt == tet.x) return coord.x;
    if(pnt == tet.y) return coord.y;
    if(pnt == tet.z) return coord.z;
    return coord.w;
}

__global__ void writeContactIndex_kernel(KeyValuePair * dstInd, 
                                    uint * srcInd, 
                                    uint n, uint bufferLength)
{
    unsigned ind = blockIdx.x*blockDim.x + threadIdx.x;
	if(ind >= bufferLength) return;
	
	if(ind < n) {
	    dstInd[ind].key = srcInd[ind];
	    dstInd[ind].value = ind >> 1;
	}
	else {
	    dstInd[ind].key = VERYVERYLARGE_INT;
	    dstInd[ind].value = VERYVERYLARGE_INT;
	}
}

__global__ void computeSplitBufLoc_kernel(uint2 * splits, 
                                    uint2 * srcPairs, 
                                    KeyValuePair * bodyPairHash, 
                                    uint bufLength)
{
    unsigned ind = blockIdx.x*blockDim.x + threadIdx.x;
	if(ind >= bufLength) return;
	
	const uint iPair = bodyPairHash[ind].value;
	if(srcPairs[iPair].x == bodyPairHash[ind].key) {
	    splits[iPair].x = ind;
	}
	else {
	    splits[iPair].y = ind;
	}
}

__global__ void countBody_kernel(uint * dstCount,
                                    KeyValuePair * srcInd, 
                                    uint maxInd)
{
    unsigned ind = blockIdx.x*blockDim.x + threadIdx.x;
	if(ind >= maxInd) return;
	
	dstCount[ind] = 0;
	
	const uint a = srcInd[ind].key;
	
	int isFirst = 0;
	
	if(ind < 1) isFirst = 1;
	else if(srcInd[ind - 1].key != a) isFirst = 1;
	
	if(!isFirst) return;
	
	dstCount[ind] = 1;

	unsigned cur = ind;
// check backward
	for(;;) {
	    if(cur == maxInd - 1) return;
	    cur++;
	    if(srcInd[cur].key != a) return;
	    dstCount[ind]++;
	}	
}

__global__ void computeSplitInvMass_kernel(float * invMass,
                                        uint2 * splits,
                                        uint2 * pairs,
                                        float * mass,
	                                    uint4 * indices,
	                                    uint * pointStart,
	                                    uint * indexStart,
	                                    uint * bodyCount,
	                                    uint4 * tet,
                                        uint maxInd)
{
    unsigned ind = blockIdx.x*blockDim.x + threadIdx.x;
	if(ind >= maxInd) return;
	
	const uint iPair = ind>>1;
	const int isRgt = (ind & 1);
	
	uint4 ia;
	uint dstInd;
	if(isRgt > 0) {
	    dstInd = splits[iPair].y;
	    ia = computePointIndex(pointStart, indexStart, indices, pairs[iPair].y);
	}
	else {
	    dstInd = splits[iPair].x;
	    ia = computePointIndex(pointStart, indexStart, indices, pairs[iPair].x);
	}
	
	tet[ind] = ia;
	
	uint n = getBodyCountAt(dstInd, bodyCount);
	
	invMass[dstInd] = (float)n / (mass[ia.x] + mass[ia.y] + mass[ia.z] + mass[ia.w]);
}

__global__ void clearDeltaVelocity_kernel(float3 * deltaLinVel, 
                                        float3 * deltaAngVel, 
                                        uint maxInd)
{
    unsigned ind = blockIdx.x*blockDim.x + threadIdx.x;
	if(ind >= maxInd) return;
	
	deltaLinVel[ind] = make_float3(0.f, 0.f, 0.f);
	// deltaAngVel[ind] = make_float3(0.f, 0.f, 0.f);
}

__global__ void solveContact_kernel(ContactConstraint* constraints,
                        float3 * deltaLinearVelocity,
	                    float3 * deltaAngularVelocity,
	                    uint2 * pairs,
                        uint2 * splits,
	                    float * splitMass,
	                    ContactData * contacts,
	                    float3 * positions,
                        float3 * velocities,
                        uint4 * indices,
                        uint * pointStarts,
                        uint * indexStarts,
                        uint maxInd,
                        float * deltaJ,
                        int maxNIt,
                        int it)
{
    __shared__ float3 sVel[SOLVECONTACT_TPB];
    __shared__ float3 sN[SOLVECONTACT_TPB];
    
    unsigned ind = blockIdx.x*blockDim.x + threadIdx.x;
	if(ind >= maxInd) return;
	
	const uint iContact = ind>>1;
	
	uint splitInd = splits[iContact].x;
	uint iBody = pairs[iContact].x;
	BarycentricCoordinate coord = constraints[iContact].coordA;
	
	if((threadIdx.x & 1)>0) {
	    splitInd = splits[iContact].y;
	    iBody = pairs[iContact].y;
	    coord = constraints[iContact].coordB;
	}

// initial velocities
    uint4 ia = computePointIndex(pointStarts, indexStarts, indices, iBody);
    
    float3 velA;
    interpolate_float3i(velA, ia, velocities, &coord);
    
	float3 nA = constraints[iContact].normal;
	float3 rA;// = contacts[iContact].localA;
	
	if((ind & 1)>0) {
	    nA = float3_reverse(nA);
	    // rA = contacts[iContact].localB;
	}
	
// N pointing inside object
// T = r X N	
	float3 torqueA = float3_cross(rA, nA);
	
	addDeltaVelocity(velA, 
        deltaLinearVelocity[splitInd],
        deltaAngularVelocity[splitInd],
        nA, rA,
        &coord);
    
    sN[threadIdx.x] = nA;
    sVel[threadIdx.x] = velA;
    __syncthreads();
    
    uint iLeft = (threadIdx.x>>1)<<1;
    uint iRight = iLeft + 1;
	
	float J = computeRelativeVelocity1(sN[iLeft], sN[iRight],
	                        sVel[iLeft], sVel[iRight]);

	J += constraints[iContact].relVel;
	J *= constraints[iContact].Minv;
	
	float prevSum = constraints[iContact].lambda;
	float updated = prevSum;
	updated += J;
	if(updated < 0.f) updated = 0.f;
	
	if((threadIdx.x & 1)==0) constraints[iContact].lambda = updated;
	
	J = updated - prevSum;
	
	if((threadIdx.x & 1)==0) deltaJ[iContact * maxNIt + it] = J;
	
	const float invMassA = splitMass[splitInd];
	
	//applyImpulse(deltaLinearVelocity[splitInd], J * invMassA, nA);
	//applyImpulse(deltaAngularVelocity[splitInd], J * invMassA, torqueA);
}

__global__ void averageVelocities_kernel(float3 * linearVelocity,
                        float3 * angularVelocity,
                        uint * bodyCount, 
                        KeyValuePair * srcInd,
                        uint maxInd)
{
    unsigned ind = blockIdx.x*blockDim.x + threadIdx.x;
	if(ind >= maxInd) return;
	
	uint c = bodyCount[ind];
	if(c < 1) return;
	
	uint a = srcInd[ind].key;
	
	float3 linSum = linearVelocity[ind];
	// float3 angSum = angularVelocity[ind];

	unsigned cur = ind;
// add up backward
	for(;;) {
	    if(cur == maxInd - 1) break;
	    cur++;
	    if(srcInd[cur].key != a) break;
	    
	    linSum = float3_add(linSum, linearVelocity[cur]);
	    // angSum = float3_add(angSum, angularVelocity[cur]);
	}

	if(c > 1) {
	    linSum = scale_float3_by(linSum, 1.f / (float)c);
	    // angSum = scale_float3_by(angSum, 1.f / (float)c);
	}
	
	linearVelocity[ind] = linSum;
	// angularVelocity[ind] = angSum;
	
	cur = ind;
// write backward
	for(;;) {
	    if(cur == maxInd - 1) break;
	    cur++;
	    if(srcInd[cur].key != a) break;
	    
	    linearVelocity[cur] = linSum;
	    // angularVelocity[cur] = angSum;
	}
}

__global__ void resetPointTetHash_kernel(KeyValuePair * pntTetHash,
	                uint maxInd)
{
    unsigned ind = blockIdx.x*blockDim.x + threadIdx.x;
	if(ind >= maxInd) return;
    pntTetHash[ind].key = VERYLARGE_INT;
}

__global__ void writePointTetHash_kernel(KeyValuePair * pntTetHash,
	                uint2 * pairs,
	                uint2 * splits,
	                uint * bodyCount,
	                uint4 * tet,
                    uint maxInd)
{
    unsigned ind = blockIdx.x*blockDim.x + threadIdx.x;
	if(ind >= maxInd) return;
	
	const uint istart = ind * 4;
	
	const unsigned iContact = ind>>1;
	
	uint splitInd = splits[iContact].x;
	uint iBody = pairs[iContact].x;
	
	if(ind & 1) {
	    splitInd = splits[iContact].y;
	    iBody = pairs[iContact].y;
	}
    
	KeyValuePair kv;  
    
	if(bodyCount[splitInd] < 1) {
// redundant
        pntTetHash[istart].key = VERYLARGE_INT;
        pntTetHash[istart + 1].key = VERYLARGE_INT;
        pntTetHash[istart + 2].key = VERYLARGE_INT;
        pntTetHash[istart + 3].key = VERYLARGE_INT;
	}
	else {
	    uint4 ia = tet[ind];
        kv.value = ind;
	    kv.key = ia.x;
        pntTetHash[istart  ] = kv;
        
        kv.key = ia.y;
	    pntTetHash[istart+1] = kv;
        
        kv.key = ia.z;
	    pntTetHash[istart+2] = kv;
	    
        kv.key = ia.w;
	    pntTetHash[istart+3] = kv;
	}
}

__global__ void updateVelocity_kernel(float3 * dstVelocity,
                    float3 * deltaLinearVelocity,
	                float3 * deltaAngularVelocity,
	                KeyValuePair * pntTetHash,
                    uint2 * pairs,
                    uint2 * splits,
                    ContactConstraint * constraints,
                    ContactData * contacts,
                    float3 * position,
                    uint4 * indices,
                    uint * objectPointStarts,
                    uint * objectIndexStarts,
                    uint maxInd)
{
    unsigned ind = blockIdx.x*blockDim.x + threadIdx.x;
	if(ind >= maxInd) return;
	
	const uint iPnt = pntTetHash[ind].key;
	
	if(iPnt > VERYLARGE_INT_M1) return;
	
	int isFirst = 0;
	
	if(ind < 1) isFirst = 1;
	else if(pntTetHash[ind - 1].key != iPnt) isFirst = 1;
	
	if(!isFirst) return;
	
	float3 sumLinVel = make_float3(0.f, 0.f, 0.f);
	float count = 0.f;
	uint cur = ind;
	uint iContact, splitInd;
	
	float3 normal;
#if ENABLE_DEFORMABILITY
    uint iBody;
    float3 r, vRot;
    float weight;
    uint4 iTet;
	BarycentricCoordinate coord;
#endif
	for(;;) {
	    iContact = pntTetHash[cur].value>>1;
	
        if(pntTetHash[cur].value & 1) {
            splitInd = splits[iContact].y;
            
#if ENABLE_DEFORMABILITY
            iBody = pairs[iContact].y;
            coord = constraints[iContact].coordB;
            r = contacts[iContact].localB;
            normal = float3_reverse(constraints[iContact].normal);
#endif
            
        }
        else {
            splitInd = splits[iContact].x;
	    
#if ENABLE_DEFORMABILITY
            iBody = pairs[iContact].x;
            coord = constraints[iContact].coordA;
            r = contacts[iContact].localA;
            normal = constraints[iContact].normal;
#endif
        }
        
        sumLinVel = float3_add(sumLinVel, deltaLinearVelocity[splitInd]);
             
#if ENABLE_DEFORMABILITY
        iTet = computePointIndex(objectPointStarts, objectIndexStarts, indices, iBody);
        weight = getPntTetWeight(iPnt, iTet, coord);

        deformMotion(vRot, r, normal, deltaAngularVelocity[splitInd]);
// weighted by vex coord        
        vRot = scale_float3_by(vRot, weight);

        sumLinVel = float3_add(sumLinVel, vRot);
#endif
        count += 1.f;
        
        if(cur == maxInd - 1) break;
	    cur++;
	    if(pntTetHash[cur].key != iPnt) break;
	}
	
	if(count > 1.f)
	    sumLinVel = scale_float3_by(sumLinVel, 1.f / count);

    //float3 a = dstVelocity[iPnt];
	//float3_add_inplace(a, sumLinVel);
    float speed = float3_length(sumLinVel);
// limit speed here
    if(speed > CSpeedLimit) float3_scale_inplace(sumLinVel, CSpeedLimit / speed);
    //dstVelocity[iPnt] = a;
    float3_add_inplace(dstVelocity[iPnt], sumLinVel);
}

extern "C" {
    
void simpleContactSolverWriteContactIndex(KeyValuePair * dstInd, 
                                    uint * srcInd, 
                                    uint n, uint bufferLength)
{
    dim3 block(512, 1, 1);
    unsigned nblk = iDivUp(bufferLength, 512);
    dim3 grid(nblk, 1, 1);
    
    writeContactIndex_kernel<<< grid, block >>>(dstInd, 
                                                srcInd,
                                                n, bufferLength);
}

void simpleContactSolverComputeSplitBufLoc(uint2 * splits, 
                                    uint2 * srcPairs, 
                                    KeyValuePair * bodyPairHash, 
                                    uint bufLength)
{
    dim3 block(512, 1, 1);
    unsigned nblk = iDivUp(bufLength, 512);
    dim3 grid(nblk, 1, 1);
    
    computeSplitBufLoc_kernel<<< grid, block >>>(splits, 
                                        srcPairs, 
                                        bodyPairHash, 
                                        bufLength);
}

void simpleContactSolverCountBody(uint * dstCount,
                                    KeyValuePair * srcInd, 
                                    uint num)
{
    dim3 block(512, 1, 1);
    unsigned nblk = iDivUp(num, 512);
    dim3 grid(nblk, 1, 1);
    
    countBody_kernel<<< grid, block >>>(dstCount,
                                     srcInd, 
                                       num);
}

void simpleContactSolverComputeSplitInverseMass(float * invMass,
                                        uint2 * splits,
                                        uint2 * pairs,
                                        float * mass,
	                                    uint4 * ind,
	                                    uint * perObjPointStart,
	                                    uint * perObjectIndexStart,
                                        uint * bodyCount, 
                                        uint4 * tet,
                                        uint bufLength)
{
    dim3 block(512, 1, 1);
    unsigned nblk = iDivUp(bufLength, 512);
    dim3 grid(nblk, 1, 1);
    
    computeSplitInvMass_kernel<<< grid, block >>>(invMass,
                                        splits,
                                        pairs,
                                        mass,
	                                    ind,
	                                    perObjPointStart,
	                                    perObjectIndexStart,
	                                    bodyCount, 
	                                    tet,
	                                    bufLength);
}

void simpleContactSolverClearDeltaVelocity(float3 * deltaLinVel, 
                                        float3 * deltaAngVel, 
                                        uint bufLength)
{
    dim3 block(512, 1, 1);
    unsigned nblk = iDivUp(bufLength, 512);
    dim3 grid(nblk, 1, 1);
    
    clearDeltaVelocity_kernel<<< grid, block >>>(deltaLinVel,
                                     deltaAngVel, 
                                       bufLength);
}

void simpleContactSolverAverageVelocities(float3 * linearVelocity,
                        float3 * angularVelocity,
                        uint * bodyCount, 
                        KeyValuePair * srcInd,
                        uint numBodies)
{
    dim3 block(512, 1, 1);
    unsigned nblk = iDivUp(numBodies, 512);
    dim3 grid(nblk, 1, 1);
    
    averageVelocities_kernel<<< grid, block >>>(linearVelocity,
                        angularVelocity,
                        bodyCount, 
                        srcInd,
                        numBodies);
}

void simpleContactSolverWritePointTetHash(KeyValuePair * pntTetHash,
	                uint2 * pairs,
	                uint2 * splits,
	                uint * bodyCount,
	                uint4 * tet,
	                uint numBodies,
	                uint bufLength)
{
    dim3 block(512, 1, 1);
    unsigned nblk = iDivUp(bufLength, 512);
    dim3 grid(nblk, 1, 1);
    
    resetPointTetHash_kernel<<< grid, block >>>(pntTetHash,
                                                bufLength);
    grid.x = iDivUp(numBodies, 512);
    writePointTetHash_kernel<<< grid, block >>>(pntTetHash,
	                pairs,
	                splits,
	                bodyCount,
	                tet,
	                numBodies);
}

}

namespace contactsolver {
	void setSpeedLimit(float x)
	{ cudaMemcpyToSymbol(CSpeedLimit, &x, 4); }
    
    void updateImpulse(float3 * dstImpulse,
                    float3 * deltaLinearVelocity,
	                float3 * deltaAngularVelocity,
	                KeyValuePair * pntTetHash,
                    uint2 * pairs,
                    uint2 * splits,
                    ContactConstraint * constraints,
                    ContactData * contacts,
                    float3 * position,
                    uint4 * indices,
                    uint * objectPointStarts,
                    uint * objectIndexStarts,
                    uint numPoints)
{
    uint tpb = 256;

    dim3 block(tpb, 1, 1);
    unsigned nblk = iDivUp(numPoints, tpb);
    dim3 grid(nblk, 1, 1);
    
    updateVelocity_kernel<<< grid, block >>>(dstImpulse,
                    deltaLinearVelocity,
	                deltaAngularVelocity,
	                pntTetHash,
                    pairs,
                    splits,
                    constraints,
                    contacts,
                    position,
                    indices,
                    objectPointStarts,
                    objectIndexStarts,
                    numPoints);
}
}


//:~
