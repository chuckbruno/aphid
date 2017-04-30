#ifndef CUFEMTETRAHEDRON_IMPLEMENT_H
#define CUFEMTETRAHEDRON_IMPLEMENT_H

#include "bvh_common.h"
#include <radixsort_implement.h>

namespace tetrahedronfem {
void setGravity(float * g);
void setWind(float * w);

void computeExternalForce(float3 * dst,
                                float * mass,
                                float3 * velocity,
                                float * wind,
                                uint maxInd);

void computeBVolume(float4 * dst, 
                    float3 * pos,
                    uint4 * tetVertices,
                    uint numTet);

void calculateRe(mat33 * dst, 
                    float3 * pos, 
                    float3 * pos0,
                    uint4 * indices,
                    float4 * BVol,
                    uint maxInd);

void internalForce(float3 * dst,
                    float3 * pos,
                    uint4 * tetvert,
                    float4 * BVol,
                    mat33 * orientation,
                    KeyValuePair * tetraInd,
                    uint * bufferIndices,
                    float4 * elasticity,
                    uint maxBufferInd,
                    uint maxInd);

void stiffnessAssembly(mat33 * dst,
                        float3 * pos,
                        uint4 * vert,
                        float4 * BVol,
                        mat33 * orientation,
                        KeyValuePair * tetraInd,
                        uint * bufferIndices,
                        float4 * elasticity,
                        uint maxBufferInd,
                        uint maxInd);

void computeElasticity(float4 * d,
                        float * alpha,
                        float Y,
                        uint maxInd,
                        float * splineV);
}

extern "C" {
    
void cuFemTetrahedron_resetRe(mat33 * d, uint maxInd);

void cuFemTetrahedron_resetStiffnessMatrix(mat33 * dst,
                                    uint maxInd);

void cuFemTetrahedron_resetForce(float3 * dst,
                                    uint maxInd);

void cuFemTetrahedron_computeRhs(float3 * rhs,
                                float3 * pos,
                                float3 * vel,
                                float * mass,
                                mat33 * stiffness,
                                uint * rowPtr,
                                uint * colInd,
                                float3 * f0,
                                float3 * externalForce,
                                float dt,
                                uint maxInd);
								
void cuFemTetrahedron_dampK(mat33 * stiffness,
                                float * mass,
                                uint * rowPtr,
                                uint * colInd,
                                float dt,
                                uint maxInd);
}
#endif        //  #ifndef CUFEMTETRAHEDRON_IMPLEMENT_H

