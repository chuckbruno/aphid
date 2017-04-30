/*
 *  GridMaker.cpp
 *  foo
 *
 *  Created by jian zhang on 7/14/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#include "GridMaker.h"
#include <iostream>

using namespace aphid;

namespace ttg {
GridMaker::GridMaker()
{}

GridMaker::~GridMaker()
{ internalClear(); }

void GridMaker::internalClear()
{
	m_grid.clear();
	m_triInds.clear();
	clearTetra();
}

void GridMaker::clearTetra()
{
	std::vector<ITetrahedron *>::iterator it = m_tets.begin();
	for(;it!=m_tets.end();++it) {
		delete *it;
	}
	m_tets.clear();
}

void GridMaker::setH(const float & x)
{ 
	m_grid.clear();
	m_grid.setGridSize(x);
}

void GridMaker::addCell(const aphid::Vector3F & p)
{
	const sdb::Coord3 c = m_grid.gridCoord((const float *)&p );
	if(m_grid.findCell(c) ) 
		return;		
/// red only
	BccNode * node15 = new BccNode;
	node15->pos = m_grid.coordToCellCenter(c);
	node15->prop = -4;
	node15->key = 15;
	m_grid.insert(c, node15 );
}

void GridMaker::buildGrid()
{
	m_grid.calculateBBox();
	std::cout<<"\n grid n cell "<<m_grid.size()
			<<"\n bbx "<<m_grid.boundingBox();
	
	m_grid.begin();
	while(!m_grid.end() ) {
		m_grid.addBlueNodes(m_grid.coordToCellCenter(m_grid.key() ) );
		m_grid.next();
	}
}

void GridMaker::buildMesh()
{ 
	clearTetra();
	m_grid.countNodes();
	m_grid.begin();
	while(!m_grid.end() ) {
		BccCell fCell(m_grid.coordToCellCenter(m_grid.key() ) );
		
		fCell.connectTetrahedrons(m_tets, m_grid.value(), &m_grid, m_grid.key() );
		m_grid.next();
	}
}

int GridMaker::numTetrahedrons() const
{ return m_tets.size(); }

BccTetraGrid * GridMaker::grid()
{ return &m_grid; }

const ITetrahedron * GridMaker::tetra(const int & i) const
{ return m_tets[i]; }

int GridMaker::numTriangles() const
{ return m_triInds.size(); }

const sdb::Coord3 & GridMaker::triangleInd(const int & i) const
{ return m_triInds[i]; }

void GridMaker::buildMesh1()
{
	m_grid.cutEdges();
	clearTetra();
	m_grid.countNodes();
	RedBlueRefine rbr;
	aphid::sdb::Array<aphid::sdb::Coord3, IFace > tris;
	m_grid.begin();
	while(!m_grid.end() ) {
		BccCell fCell(m_grid.coordToCellCenter(m_grid.key() ) );
		
		fCell.connectRefinedTetrahedrons(m_tets, tris, rbr, m_grid.value(), &m_grid, m_grid.key() );
		m_grid.next();
	}
	
	m_triInds.clear();
	
	tris.begin();
	while(!tris.end() ) {
	
		m_triInds.push_back(tris.key() );
		tris.next();
	}
	
	tris.clear();
}

}
