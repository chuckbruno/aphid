/*
 *  MlCluster.cpp
 *  mallard
 *
 *  Created by jian zhang on 12/7/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "MlCluster.h"
#include "MlCalamusArray.h"
#include <AccPatchMesh.h>
#include <MlFeather.h>
#include <CollisionRegion.h>
MlCluster::MlCluster() 
{
    m_sampleIndices = 0;
    m_angleStart = 0;
    m_angles = 0;
	m_sampleNSegs = 0;
	m_sampleBend = 0;
}

MlCluster::~MlCluster() 
{
	if(m_sampleIndices) delete[] m_sampleIndices;
    if(m_angleStart) delete[] m_angleStart;
    if(m_angles) delete[] m_angles;
	if(m_sampleNSegs) delete[] m_sampleNSegs;
	if(m_sampleBend) delete[] m_sampleBend;
	m_sampleIndices = 0;
    m_angleStart = 0;
    m_angles = 0;
	m_sampleNSegs = 0;
	m_sampleBend = 0;
}

void MlCluster::setK(const unsigned & k)
{
	KMeansClustering::setK(k);
	const int k2 = K()+1;
	if(m_sampleIndices) delete[] m_sampleIndices;
	m_sampleIndices = new unsigned[k2];
	if(m_angleStart) delete[] m_angleStart;
	m_angleStart = new unsigned[k2];
	if(m_sampleNSegs) delete[] m_sampleNSegs;
	m_sampleNSegs = new short[k2];
	if(m_sampleBend) delete[] m_sampleBend;
	m_sampleBend = new float[k2];
}

void MlCluster::compute(MlCalamusArray * calamus, AccPatchMesh * mesh, unsigned begin, unsigned end)
{
	if(begin >= end) {
		setValid(0);
		return;
	}
	const unsigned n = end - begin;
	unsigned i;
	setN(n);
	if(n < 4) {
		setK(n);
		resetGroup();
		for(i = 0; i < N(); i++) m_sampleIndices[i] = begin + i;
		createAngles(calamus);
		setValid(1);
		return;
	}
	const unsigned k = 3 + (n - 1) / 4;
	setK(k);
	unsigned j;
	float d;
	Vector3F pos;
	const unsigned faceIdx = calamus->asCalamus(begin)->faceIdx();
	for(i = 0; i < k; i++) {
		MlCalamus * c = calamus->asCalamus(begin + i);
		mesh->pointOnPatch(faceIdx, c->patchU(), c->patchV(), pos);
		setInitialGuess(i, pos);
	}
	
	for(j = 0; j < 8; j++) {
		preAssign();
		for(i = begin; i < end; i++) {
			MlCalamus * c = calamus->asCalamus(i);
			mesh->pointOnPatch(faceIdx, c->patchU(), c->patchV(), pos);
			assignToGroup(i - begin, pos);
		}
		d = moveCentroids();
		if(d < 10e-3) break;
	}
	
	for(unsigned i = 0; i < K(); i++)
		assignGroupSample(calamus, mesh, begin, i);
	createAngles(calamus);
	setValid(1);
}

void MlCluster::assignGroupSample(MlCalamusArray * calamus, AccPatchMesh * mesh, unsigned begin, unsigned grp)
{
	Vector3F pos;
	float d, minD = 10e8;
	for(unsigned i = 0; i < N(); i++) {
		if(group(i) != grp) continue;
		MlCalamus * c = calamus->asCalamus(begin + i);
		mesh->pointOnPatch(c->faceIdx(), c->patchU(), c->patchV(), pos);
		d = Vector3F(pos, groupCenter(grp)).length();
		if(d < minD) {
			minD = d;
			m_sampleIndices[grp] = begin + i;
		}
	}
}

void MlCluster::createAngles(MlCalamusArray * calamus)
{
	unsigned numAngles = 0;
	for(unsigned i = 0; i < K(); i++) {
		m_angleStart[i] = numAngles;
		MlCalamus * c = calamus->asCalamus(m_sampleIndices[i]);
		m_sampleNSegs[i] = c->feather()->numSegment();
		numAngles += m_sampleNSegs[i];
	}
	
	if(m_angles) delete[] m_angles;
	m_angles = new Float2[numAngles];
}

Float2 * MlCluster::angles(unsigned idx) const
{
    return &m_angles[m_angleStart[idx]];
}

unsigned MlCluster::sampleIdx(unsigned idx) const
{
	return m_sampleIndices[idx];
}

void MlCluster::recordAngles(MlCalamus * c, unsigned idx)
{
	Float2 * dst = angles(idx);
	Float2 * src = c->feather()->angles();
	const short ns = c->featherNumSegment();
	for(short i = 0; i < ns; i++) dst[i] = src[i];
	
	m_sampleBend[idx] = c->feather()->bendDirection();
}

void MlCluster::reuseAngles(MlCalamus * c, unsigned idx)
{
	Float2 * src = angles(idx);
	Float2 * dst = c->feather()->angles();
	const short ns = c->featherNumSegment();
	for(short i = 0; i < ns; i++) dst[i] = src[i];
}

short MlCluster::sampleNSeg(unsigned idx) const
{
	return m_sampleNSegs[idx];
}

float MlCluster::sampleBend(unsigned idx) const
{
	return m_sampleBend[idx];
}
