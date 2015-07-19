/*
 *  TriangleDifference.cpp
 *  aphid
 *
 *  Created by jian zhang on 7/15/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 *  reference: Deformation Transfer for Triangle Meshes by Robert W. Sumner Jovan Popovic
 *  
 */

#include "TriangleDifference.h"
#include "ATriangleMesh.h"
#include "BaseBuffer.h"
#include "Matrix33F.h"
#include <cmath>
#include <iostream>
TriangleDifference::TriangleDifference(ATriangleMesh * target) : ModelDifference(target)
{
    const unsigned n = target->numTriangles();
	m_V = new BaseBuffer;
	m_V->create(n * 36);
    m_Q = new BaseBuffer;
    m_Q->create(n * 36);
    m_binded = new BaseBuffer;
    m_binded->create(n * 4);
    m_C = new BaseBuffer;
    m_C->create(n * 12);
	Matrix33F * v = undeformedV();
	computeV(v, target);
	unsigned i=0;
	for(;i<n;i++) v[i].inverse();
    unsigned * b = binded();
    for(i=0;i<n;i++) b[i] = 0;
    computeC(undeformedC(), target);
}

TriangleDifference::~TriangleDifference() 
{
	delete m_V;
    delete m_Q;
}

void TriangleDifference::computeV(Matrix33F * dst, ATriangleMesh * mesh) const
{
	const unsigned n = mesh->numTriangles();
	Vector3F * p = mesh->points();
	Vector3F v1, v2, v3, v4;
	unsigned i=0;
	for(;i<n;i++) {
		unsigned * vi = mesh->triangleIndices(i);
		v1 = p[vi[0]];
		v2 = p[vi[1]];
		v3 = p[vi[2]];
		v4 = getV4(v1, v2, v3);
		
		dst[i].fill(v2-v1, v3-v1, v4-v1);
	}
}

Vector3F TriangleDifference::getV4(const Vector3F & v1, const Vector3F & v2, const Vector3F & v3) const
{
	Vector3F nor = (v2 - v1).cross(v3 - v1);
	nor *= 1.f/sqrt(nor.length());
	return v1 + nor;
}

Matrix33F * TriangleDifference::undeformedV()
{ return (Matrix33F *)m_V->data(); }

Matrix33F * TriangleDifference::Q()
{ return (Matrix33F *)m_Q->data(); }

unsigned * TriangleDifference::binded()
{ return (unsigned *)m_binded->data(); }

Vector3F * TriangleDifference::undeformedC()
{ return (Vector3F *)m_C->data(); }

void TriangleDifference::computeQ(ATriangleMesh * mesh)
{
    Matrix33F * dst = Q();
    unsigned * b = binded();
    const unsigned n = mesh->numTriangles();
	Vector3F * p = mesh->points();
	Vector3F v1, v2, v3, v4;
	Matrix33F deformedV;
	unsigned i = 0;
	for(;i<n;i++) {
        if(b[i]<1) continue;
        
		unsigned * vi = mesh->triangleIndices(i);
		v1 = p[vi[0]];
		v2 = p[vi[1]];
		v3 = p[vi[2]];
		v4 = getV4(v1, v2, v3);
		deformedV.fill(v2-v1, v3-v1, v4-v1);
		
		dst[i] = deformedV * undeformedV()[i];
	}
}

void TriangleDifference::requireQ(AGenericMesh * m)
{
    std::map<unsigned, unsigned> inds;
    m->getAnchorInd(inds);
    std::cout<<" n binded tris "<<inds.size();
    unsigned * b = binded();
    std::map<unsigned, unsigned>::const_iterator it = inds.begin();
    for(;it!=inds.end();++it) b[it->first] = 1;
}

void TriangleDifference::computeC(Vector3F * dst, ATriangleMesh * mesh) const
{
    const unsigned n = mesh->numTriangles();
	Vector3F * p = mesh->points();
	Vector3F v1, v2, v3;
	unsigned i=0;
	for(;i<n;i++) {
		unsigned * vi = mesh->triangleIndices(i);
		v1 = p[vi[0]];
		v2 = p[vi[1]];
		v3 = p[vi[2]];
		
		dst[i] = v1 + v2 + v3;
        dst[i] *= 1.f / 3.f;
	}
}
//:~