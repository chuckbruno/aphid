/*
 *  TetraField.cpp
 *  foo
 *
 *  Created by jian zhang on 7/14/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#include "TetraField.h"
#include <iostream>

using namespace aphid;

namespace ttg {

TetraField::TetraField()
{}

TetraField::~TetraField()
{}

void TetraField::buildGraph()
{
	sdb::Sequence<sdb::Coord2> egs;
	
	int v0, v1, v2, v3;
	const int n = numTetrahedrons();
	int i = 0;
	for(; i<n; ++i) {
		const ITetrahedron * t = tetra(i);
		v0 = t->iv0;
		v1 = t->iv1;
		v2 = t->iv2;
		v3 = t->iv3;
		egs.insert(sdb::Coord2(v0, v1).ordered() );
		egs.insert(sdb::Coord2(v0, v2).ordered() );
		egs.insert(sdb::Coord2(v0, v3).ordered() );
		egs.insert(sdb::Coord2(v1, v2).ordered() );
		egs.insert(sdb::Coord2(v1, v3).ordered() );
		egs.insert(sdb::Coord2(v2, v3).ordered() );
	}
	
	std::map<int, std::vector<int> > vvemap;
	
	int c = 0;
	egs.begin();
	while(!egs.end() ) {
	
		int v0 = egs.key().x;
		vvemap[v0].push_back(c);
		
		int v1 = egs.key().y;
		vvemap[v1].push_back(c);
		
		c++;
		egs.next();
	}
	
	std::vector<int> edgeBegins;
	std::vector<int> edgeInds;
	
	int nvve = 0;
	std::map<int, std::vector<int> >::iterator it = vvemap.begin();
	for(;it!=vvemap.end();++it) {
		edgeBegins.push_back(nvve);
		
		pushIndices(it->second, edgeInds);
		nvve += (it->second).size();
		
		it->second.clear();
	}
	
	int nv = grid()->numNodes();
	int ne = egs.size();
	int ni = edgeInds.size();
	ADistanceField::create(nv, ne, ni);
	
	extractGridNodes<BccTetraGrid, BccNode >(nodes(), grid() );
	extractEdges(&egs);
	extractEdgeBegins(edgeBegins);
	extractEdgeIndices(edgeInds);
	
	vvemap.clear();
	edgeBegins.clear();
	edgeInds.clear();
	egs.clear();
	
	calculateEdgeLength();
	
}

void TetraField::pushIndices(const std::vector<int> & a,
							std::vector<int> & b)
{
	std::vector<int>::const_iterator it = a.begin();
	for(;it!=a.end();++it) 
		b.push_back(*it);
}

void TetraField::verbose()
{
	std::cout<<"\n grid n tetra "<<numTetrahedrons()
		<<"\n grid n node "<<numNodes()
		<<"\n grid n edge "<<numEdges()
		<<"\n grid n edge ind "<<numEdgeIndices()
		<<"\n grid edge length min/max "<<minEdgeLength()
								<<"/"<<maxEdgeLength();
	if(numTriangles() > 0)
		std::cout<<"\n n triangle "<<numTriangles();
	std::cout.flush();
}

void TetraField::getTetraShape(cvx::Tetrahedron & b, const int & i) const
{ 
	const ITetrahedron * a = tetra(i);
	b.set(nodes()[a->iv0].pos,
			nodes()[a->iv1].pos,
			nodes()[a->iv2].pos,
			nodes()[a->iv3].pos);
}

void TetraField::getTriangleShape(cvx::Triangle & t, const int & i) const
{
	const sdb::Coord3 & c = triangleInd(i);
	t.set(nodes()[c.x].pos,
			nodes()[c.y].pos,
			nodes()[c.z].pos);
}

void TetraField::markTetraOnFront(const int & i)
{
	const ITetrahedron * a = tetra(i);
	nodes()[a->iv0].label = sdf::StFront;
	nodes()[a->iv1].label = sdf::StFront;
	nodes()[a->iv2].label = sdf::StFront;
	nodes()[a->iv3].label = sdf::StFront;
	addDirtyEdge(a->iv0, a->iv1);
	addDirtyEdge(a->iv0, a->iv2);
	addDirtyEdge(a->iv0, a->iv3);
	addDirtyEdge(a->iv1, a->iv2);
	addDirtyEdge(a->iv1, a->iv3);
	addDirtyEdge(a->iv2, a->iv3);
}

void TetraField::buildRefinedMesh()
{ 
	obtainGridNodeVal<BccTetraGrid, BccNode >(nodes(), grid() );
	buildMesh1();
}

bool TetraField::checkTetraVolume()
{ return checkTetraVolumeExt(nodes() ); }

}