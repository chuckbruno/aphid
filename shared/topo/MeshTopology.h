/*
 *  MeshTopology.h
 *  fit
 *
 *  Created by jian zhang on 4/22/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include <math/Vector3F.h>
#include <vector>
#include <boost/scoped_array.hpp>

namespace aphid {

class VertexAdjacency;
class Facet;
class Edge;
class BaseMesh;

class MeshTopology {
public:
    MeshTopology(Vector3F * pos, Vector3F * nor, int * tri, const int & numV, const int & numTri);
	MeshTopology(BaseMesh * mesh);
	virtual ~MeshTopology();
	
	void cleanup();
	
	void update(const int & nv);
	void getDifferentialCoord(const int & vertexId, Vector3F & dst);
	void calculateWeight();
	void calculateNormal();
	void calculateSmoothedNormal(Vector3F * dst);
	
	void calculateVertexNormal(const int & i);

	VertexAdjacency * getTopology() const;
	VertexAdjacency & getAdjacency(unsigned idx) const;
	Facet * getFacet(unsigned idx) const;
	Edge * getEdge(unsigned idx) const;
	Edge * findEdge(unsigned a, unsigned b) const;
	Edge * parallelEdge(Edge * src) const;
	unsigned growAroundQuad(unsigned idx, std::vector<unsigned> & dst) const;
	void checkVertexValency() const;
private:
	char parallelEdgeInQuad(unsigned *indices, unsigned v0, unsigned v1, unsigned & a, unsigned & b) const;
	boost::scoped_array<VertexAdjacency> m_adjacency;
	std::vector<Facet *> m_faces;
	BaseMesh * m_mesh;
	Vector3F * m_pos;
	Vector3F * m_nor;
};

}