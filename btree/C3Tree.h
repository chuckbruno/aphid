/*
 *  C3Tree.h
 *  btree
 *
 *  Created by jian zhang on 5/5/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 *  3-dimensional map to a list of vertexP resides each grid
 */

#pragma once

#include <Ordered.h>
#include <List.h>
#include <BoundingBox.h>
namespace aphid {
namespace sdb {

class C3Tree : public Ordered<Coord3, VertexP>
{
public:
    C3Tree(Entity * parent = NULL);
    void insert(const VertexP & v);
	void remove(const VertexP & v);
	void displace(const VertexP & v, const Vector3F & pref);
	List<VertexP> * find(float * p);
    
	void setGridSize(const float & x);
	const float gridSize() const;
	
	const BoundingBox gridBoundingBox() const;
	const BoundingBox boundingBox() const;
	void calculateBBox();
	List<VertexP> * verticesInGrid();
	const Coord3 gridCoord(const float * p) const;
	
private:
	const Coord3 inGrid(const V3 & p) const;
	const BoundingBox coordToGridBBox(const Coord3 & c) const;
	void updateBBox(const BoundingBox & b);
	
	BoundingBox m_bbox;
	float m_gridSize;
};
} // end namespace sdb
}