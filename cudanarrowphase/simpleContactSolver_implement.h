#ifndef SIMPLECONTACTSOLVER_IMPLEMENT_H
#define SIMPLECONTACTSOLVER_IMPLEMENT_H

#include "bvh_common.h"
#include "radixsort_implement.h"

struct ContactConstraint {
    BarycentricCoordinate coordA;
    BarycentricCoordinate coordB;
    float3 normal;
    float lambda;
    float Minv;
    float relVel;
};

extern "C" {
void simpleContactSolverWriteContactIndex(KeyValuePair * dstInd, 
                                    uint * srcInd, 
                                    uint n, uint bufferLength);

void simpleContactSolverCountBody(uint * dstCount,
                                    KeyValuePair * srcInd, 
                                    uint num);

void simpleContactSolverComputeSplitBufLoc(uint2 * splits, 
                                    uint2 * srcPairs, 
                                    KeyValuePair * bodyPairHash, 
                                    uint bufLength);

void simpleContactSolverComputeSplitInverseMass(float * invMass, 
                                        uint2 * splits,
                                        uint2 * pairs,
                                        float * mass,
	                                    uint4 * ind,
	                                    uint * perObjPointStart,
	                                    uint * perObjectIndexStart,
                                        uint * bodyCount, 
                                        uint4 * tet,
                                        uint bufLength);

void simpleContactSolverClearDeltaVelocity(float3 * deltaLinVel, 
                                        float3 * deltaAngVel, 
                                        uint bufLength);

void simpleContactSolverAverageVelocities(float3 * linearVelocity,
                        float3 * angularVelocity,
                        uint * bodyCount, 
                        KeyValuePair * srcInd,
                        uint numBodies);

void simpleContactSolverWritePointTetHash(KeyValuePair * pntTetHash,
	                uint2 * pairs,
	                uint2 * splits,
	                uint * bodyCount,
	                uint4 * tet,
	                uint numBodies,
	                uint bufLength);

}

namespace contactsolver {
	void setSpeedLimit(float x);
    
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
                    uint numPoints);
}

namespace contactconstraint {
    void prepareNoPenetratingContact(ContactConstraint* constraints,
        float3 * contactLinearVel,
                                        uint2 * splits,
                                        uint2 * pairs,
                                        float3 * pos,
                                        float3 * vel,
                                        float3 * impulse,
                                        float * splitMass,
                                        ContactData * contacts,
                                        uint4 * tetind,
                                        uint numContacts);
}

namespace collisionres {
    void resolveCollision(ContactConstraint* constraints,
                        float3 * contactLinearVelocity,
                        float3 * deltaLinearVelocity,
	                    uint2 * pairs,
                        uint2 * splits,
	                    float * splitMass,
	                    ContactData * contacts,
	                    uint numContacts);
	
	void resolveFriction(ContactConstraint* constraints,
                        float3 * contactLinearVelocity,
                        float3 * deltaLinearVelocity,
	                    uint2 * pairs,
                        uint2 * splits,
	                    float * splitMass,
	                    ContactData * contacts,
	                    uint numContacts);
	
}
#endif        //  #ifndef SIMPLECONTACTSOLVER_IMPLEMENT_H
