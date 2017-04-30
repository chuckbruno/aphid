/*
 *  BccMesh.cpp
 *  testbcc
 *
 *  Created by jian zhang on 4/27/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#include "BccMesh.h"
#include <GeometryArray.h>
#include <KdIntersection.h>
#include <BccGrid.h>

BccMesh::BccMesh() 
{
}

BccMesh::~BccMesh() 
{
	delete m_grid;
	delete m_intersect;
}

void BccMesh::create(GeometryArray * geoa, KdIntersection * anchorIntersect, int level)
{
	m_intersect = new KdIntersection;
	const unsigned n = geoa->numGeometries();
	unsigned i=0;
	for(;i<n;i++) m_intersect->addGeometry(geoa->geometry(i));
	m_intersect->create();
	
	m_grid = new BccGrid(m_intersect->getBBox()); 
	std::cout<<" finest grid size "<<(m_grid->span() / (float)(1<<level));
    
	m_grid->create(m_intersect, level);
	
	unsigned nt = m_grid->numTetrahedrons();
	unsigned np = m_grid->numTetrahedronVertices();
	
	std::cout<<" n tetrahedrons "<<nt<<"\n";
	std::cout<<" n vertices "<<np<<"\n";
	
	setNumPoints(np);
	setNumIndices(nt * 4);
	createBuffer(np, nt * 4);
	
	resetAnchors(np);
	
	m_grid->addAnchors(anchors(), anchorIntersect);
	m_grid->extractTetrahedronMeshData(points(), indices());
}
