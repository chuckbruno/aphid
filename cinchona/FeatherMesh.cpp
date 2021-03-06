/*
 *  FeatherMesh.cpp
 *  
 *
 *  Created by jian zhang on 1/3/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#include "FeatherMesh.h"
#include <math/Matrix44F.h>

using namespace aphid;

FeatherMesh::FeatherMesh(const float & c,
			const float & m,
			const float & p,
			const float & t) : AirfoilMesh(c, m, p, t)
{ 
	m_leadingEdgeVertices = NULL; 
	m_leadingEdgeIndices = NULL;
}

FeatherMesh::~FeatherMesh()
{ 
	if(m_leadingEdgeVertices) {
		delete[] m_leadingEdgeVertices; 
	}
	if(m_leadingEdgeIndices) {
		delete[] m_leadingEdgeIndices;
	}
}

void FeatherMesh::create(const int & gx,
				const int & gy)
{
	tessellate(gx, gy);
	flipAlongChord();
	
	Matrix44F rot;
	rot.setOrientations(Vector3F(1,0,0),
						Vector3F(0,0,-1),
						Vector3F(0,1,0) );
			
	Vector3F * p = points();
	Vector3F * vn = vertexNormals();
	const int n = numPoints();
	for(int i=0;i<n;++i) {
		p[i] = rot.transform(p[i]);
		vn[i].set(0,1,0);
	}
	
	if(m_leadingEdgeVertices) {
		delete[] m_leadingEdgeVertices;
	}
	
	if(m_leadingEdgeIndices) {
		delete[] m_leadingEdgeIndices;
	}
	
	m_leadingEdgeVertices = new Vector3F[gx+1];
	m_leadingEdgeIndices = new int[gx+1];
	m_leadingEdgeVertices[0] = p[0];
	m_leadingEdgeIndices[0] = 0;
	m_nvprow = gy*2+1;
	m_1strow = 1;
	int it = 1;
	for(int i=0;i<gx-1;++i) {
		m_leadingEdgeVertices[it] = p[1 + i*m_nvprow];
		m_leadingEdgeIndices[it] = 1 + i*m_nvprow;
		it++;
	}
	m_leadingEdgeVertices[it] = p[numPoints() - 1];
	m_leadingEdgeIndices[it] = numPoints() - 1;
	m_numLeadingEdgeVertices = gx+1;
}

const int & FeatherMesh::numLeadingEdgeVertices() const
{ return m_numLeadingEdgeVertices; }

const Vector3F * FeatherMesh::leadingEdgeVertices() const
{ return m_leadingEdgeVertices; }

const int * FeatherMesh::leadingEdgeIndices() const
{ return m_leadingEdgeIndices; }

const int & FeatherMesh::numVerticesPerRow() const
{ return m_nvprow; }

const int & FeatherMesh::vertexFirstRow() const
{ return m_1strow; }

int FeatherMesh::numVertexRows() const
{ return numPoints() / m_nvprow; }

void FeatherMesh::flipZ()
{
	const int n = numPoints();
	for(int i=0;i<n;++i) {
		points()[i].z *= -1.f;
	}
	reverseTriangleNormals();
	
}
