/*
 *  BvhTriangleMesh.cpp
 *  cudabvh
 *
 *  Created by jian zhang on 1/14/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#include "BvhTriangleMesh.h"
#include <CUDABuffer.h>
#include "CudaLinearBvh.h"
#include "createBvh_implement.h"
#include "reduceBox_implement.h"

BvhTriangleMesh::BvhTriangleMesh() 
{
	m_vertices = new BaseBuffer;
	m_triangleIndices = new BaseBuffer;
}

BvhTriangleMesh::~BvhTriangleMesh() 
{

}

void BvhTriangleMesh::initOnDevice()
{
	m_verticesOnDevice = new CUDABuffer;
	m_verticesOnDevice->create(m_vertices->bufferSize());
	m_triangleIndicesOnDevice = new CUDABuffer;
	m_triangleIndicesOnDevice->create(m_triangleIndices->bufferSize());
	m_triangleIndicesOnDevice->hostToDevice(m_triangleIndices->data(), m_triangleIndices->bufferSize());
	
	bvh()->setNumLeafNodes(numTriangles());
	CollisionObject::initOnDevice();
}

void BvhTriangleMesh::update()
{ 
    formTriangleAabbs();
    CollisionObject::update(); 
}

void BvhTriangleMesh::formTriangleAabbs()
{
    void * cvs = verticesOnDevice();
    void * tri = triangleIndicesOnDevice();
    void * dst = bvh()->leafAabbs();
    bvhCalculateLeafAabbsTriangle((Aabb *)dst, (float3 *)cvs, (uint3 *)tri, numTriangles());
}

void BvhTriangleMesh::createVertices(unsigned n)
{
	m_numVertices = n;
	m_vertices->create(n * 12);
}

void BvhTriangleMesh::createTriangles(unsigned n)
{
	m_numTriangles = n;
	m_triangleIndices->create(n * 3 * 4);
}

const unsigned BvhTriangleMesh::numVertices() const 
{ return m_numVertices; }

const unsigned BvhTriangleMesh::numTriangles() const
{ return m_numTriangles; }

const unsigned BvhTriangleMesh::numTriangleFaceVertices() const
{ return numTriangles() * 3; }

BaseBuffer * BvhTriangleMesh::vertexBuffer()
{ return m_vertices; }

CUDABuffer * BvhTriangleMesh::vertexBufferOnDevice()
{ return m_verticesOnDevice; }

void * BvhTriangleMesh::verticesOnDevice()
{ return m_verticesOnDevice->bufferOnDevice(); }

Vector3F * BvhTriangleMesh::vertices()
{ return (Vector3F *)m_vertices->data(); }

void * BvhTriangleMesh::triangleIndicesOnDevice()
{ return m_triangleIndicesOnDevice->bufferOnDevice(); }

unsigned * BvhTriangleMesh::triangleIndices()
{ return (unsigned *)m_triangleIndices->data(); }

void BvhTriangleMesh::getVerticesOnDevice(BaseBuffer * dst)
{ m_verticesOnDevice->deviceToHost(dst->data(), m_verticesOnDevice->bufferSize()); }

void BvhTriangleMesh::deviceToHost()
{ m_verticesOnDevice->deviceToHost(m_vertices->data(), m_verticesOnDevice->bufferSize()); }

