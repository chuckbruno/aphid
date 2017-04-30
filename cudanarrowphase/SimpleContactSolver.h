#ifndef SIMPLECONTACTSOLVER_H
#define SIMPLECONTACTSOLVER_H

/*
 *  SimpleContactSolver.h
 *  testnarrowpahse
 *
 *  Created by jian zhang on 3/8/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */
#include <CudaReduction.h>
class CUDABuffer;
class SimpleContactSolver : public CudaReduction {
public:
	SimpleContactSolver();
	virtual ~SimpleContactSolver();
	void initOnDevice();
	void solveContacts(unsigned numContacts,
						CUDABuffer * contactBuf,
						CUDABuffer * pairBuf,
						void * objectData);
						
						
	CUDABuffer * contactPairHashBuf();
	CUDABuffer * bodySplitLocBuf();
	CUDABuffer * constraintBuf();
	CUDABuffer * deltaLinearVelocityBuf();
	CUDABuffer * deltaAngularVelocityBuf();
	CUDABuffer * pntTetHashBuf();
	CUDABuffer * splitInverseMassBuf();
	
	const unsigned numContacts() const;
	
	void setSpeedLimit(float x);
private:
	CUDABuffer * m_sortedInd[2];
	CUDABuffer * m_splitPair;
	CUDABuffer * m_bodyCount;
	CUDABuffer * m_splitInverseMass;
	CUDABuffer * m_constraint;
	CUDABuffer * m_deltaLinearVelocity;
	CUDABuffer * m_deltaAngularVelocity;
	CUDABuffer * m_contactLinearVelocity;
	CUDABuffer * m_relVel;
	CUDABuffer * m_pntTetHash[2];
	CUDABuffer * m_bodyTetInd;
	unsigned m_numContacts;
};
#endif        //  #ifndef SIMPLECONTACTSOLVER_H
