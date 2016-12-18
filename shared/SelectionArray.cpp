/*
 *  SelectionArray.cpp
 *  lapl
 *
 *  Created by jian zhang on 3/18/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "SelectionArray.h"
#include "Primitive.h"
#include "BaseMesh.h"
#include <topo/VertexPath.h>
#include <topo/ComponentConversion.h>
#include <topo/MeshTopology.h>

namespace aphid {

SelectionArray::SelectionArray() 
{
	m_vertexPath = new VertexPath;
	m_compconvert = new ComponentConversion;
}

SelectionArray::~SelectionArray() {}

void SelectionArray::reset() 
{
	m_prims.clear();
	m_vertexIds.clear();
	m_edgeIds.clear();
	m_faceIds.clear();
}

void SelectionArray::add(Geometry * geo, unsigned icomp, const Vector3F & atP)
{
    m_geometry = geo;
	BaseMesh * mesh = (BaseMesh *)geo;
	if(getComponentFilterType() == PrimitiveFilter::TFace) {
	    if(isFaceSelected(icomp)) return;
	    m_faceIds.push_back(icomp);
	    
	    unsigned vertexId[3];
		mesh->getTriangle(icomp, vertexId);
	
        for(int i = 0; i < 3; i++)
            addVertex(vertexId[i], mesh->getVertices()[vertexId[i]]);
    }
    else {
		if(isVertexSelected(icomp)) return;
		
		if(numVertices() < 1) {
			addVertex(icomp, mesh->getVertices()[icomp]);
			return;
		}
		
		if(!m_needVertexPath) {
		    addVertex(icomp, atP);
			return;
		}
		
		unsigned startVert = lastVertexId();
		unsigned endVert = icomp;
		m_vertexPath->create(startVert, endVert);
		
		for(unsigned i = 0; i < m_vertexPath->numVertices(); i++)
			addVertex(m_vertexPath->vertex(i), mesh->getVertices()[m_vertexPath->vertex(i)]);
    }
}

unsigned SelectionArray::numPrims() const
{
	return (unsigned)m_prims.size();
}

Primitive * SelectionArray::getPrimitive(const unsigned & idx) const
{
	return m_prims[idx];
}

unsigned SelectionArray::numVertices() const
{
	return (unsigned)m_vertexIds.size();
}

unsigned SelectionArray::getVertexId(const unsigned & idx) const
{
	return m_vertexIds[idx];
}

unsigned SelectionArray::lastVertexId() const
{
	return m_vertexIds[m_vertexIds.size() - 1];
}

Vector3F SelectionArray::getVertexP(const unsigned & idx) const
{
	return m_compconvert->vertexPosition(m_vertexIds[idx]);
}

bool SelectionArray::isVertexSelected(unsigned idx) const
{
	std::vector<unsigned>::const_iterator vIt;
	for(vIt = m_vertexIds.begin(); vIt != m_vertexIds.end(); ++vIt) {
		if((*vIt) == idx)
			return true;
	}
	return false;
}

void SelectionArray::addVertex(unsigned idx, const Vector3F & atP)
{
	if(!isVertexSelected(idx)) {
		m_vertexIds.push_back(idx);
	}
}

bool SelectionArray::isFaceSelected(unsigned idx) const
{
	std::vector<unsigned>::const_iterator it;
	for(it = m_faceIds.begin(); it != m_faceIds.end(); ++it) {
		if((*it) == idx)
			return true;
	}
	return false;
}

Geometry * SelectionArray::getGeometry() const
{
    return m_geometry;
}

unsigned SelectionArray::numFaces() const
{
    return (unsigned)m_faceIds.size();
}

unsigned SelectionArray::getFaceId(const unsigned & idx) const
{
    return m_faceIds[idx];
}

void SelectionArray::setTopology(MeshTopology * topo)
{
	m_vertexPath->setTopology(topo);
	m_compconvert->setTopology(topo);
}

void SelectionArray::grow()
{
	if(numVertices() < 2) return;
	BaseMesh * mesh = (BaseMesh *)m_geometry;
	std::vector<unsigned>::iterator it = m_vertexIds.end();
	it--;
	unsigned endVert = *it;
	it--;
	unsigned startVert = *it;
	unsigned nextVert;
	if(m_vertexPath->grow(startVert, endVert, nextVert))
		addVertex(nextVert, mesh->getVertices()[nextVert]);
}

void SelectionArray::shrink()
{
	if(numVertices() > 0) {
		std::vector<unsigned>::iterator it = m_vertexIds.end();
		--it;
		m_vertexIds.erase(it);
	}
}

void SelectionArray::enableVertexPath()
{
	m_needVertexPath = true;
}

void SelectionArray::disableVertexPath()
{
	m_needVertexPath = false;
}

bool SelectionArray::hasVertexPath() const
{
	return m_needVertexPath;
}

void SelectionArray::asPolygonRing(std::vector<unsigned> & polyIds, std::vector<unsigned> & vppIds) const
{
    std::vector<unsigned> srcedges;
    asEdges(srcedges);
	std::vector<unsigned> ringedges;
    std::vector<unsigned>::const_iterator it;
    for(it = srcedges.begin(); it != srcedges.end(); ++it) {
        m_compconvert->edgeRing(*it, ringedges);
    }
	
	m_compconvert->edgeToPolygon(ringedges, polyIds, vppIds);
}

void SelectionArray::asEdges(std::vector<unsigned> & dst) const
{
    if(getComponentFilterType() == PrimitiveFilter::TVertex) {
		m_compconvert->vertexToEdge(m_vertexIds, dst);
	}
}

void SelectionArray::asEdges(std::vector<Edge *> & dst) const
{
    if(getComponentFilterType() == PrimitiveFilter::TVertex) {
		m_compconvert->vertexToEdge(m_vertexIds, dst);
	}
}

void SelectionArray::asVertices(std::vector<unsigned> & dst) const
{
	if(getComponentFilterType() == PrimitiveFilter::TVertex) {
		std::vector<unsigned>::const_iterator it;
		for(it = m_vertexIds.begin(); it != m_vertexIds.end(); ++it) {
			dst.push_back(*it);
		}
	}
}

void SelectionArray::asPolygons(std::vector<unsigned> & polyIds, std::vector<unsigned> & vppIds) const
{
	if(getComponentFilterType() == PrimitiveFilter::TFace) {
		m_compconvert->facetToPolygon(m_faceIds, polyIds);
	}
	else if(getComponentFilterType() == PrimitiveFilter::TVertex) {
		m_compconvert->vertexToPolygon(m_vertexIds, polyIds, vppIds);
	}
}

}
//:~
