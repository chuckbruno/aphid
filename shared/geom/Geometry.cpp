/*
 *  Geometry.cpp
 *  kdtree
 *
 *  Created by jian zhang on 10/23/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <geom/Geometry.h>

namespace aphid {

Geometry::Geometry() {}
Geometry::~Geometry() {}

const unsigned Geometry::numComponents() const 
{ return 0; }

const BoundingBox Geometry::calculateBBox() const 
{ return BoundingBox(); }

const BoundingBox Geometry::calculateBBox(unsigned icomponent) const
{ return BoundingBox(); }

bool Geometry::intersectBox(const BoundingBox & box)
{ return false; }

bool Geometry::intersectTetrahedron(const Vector3F * tet)
{ return false; }

bool Geometry::intersectRay(const Ray * r)
{ return false; }

bool Geometry::intersectBox(unsigned icomponent, const BoundingBox & box)
{ return false; }

bool Geometry::intersectTetrahedron(unsigned icomponent, const Vector3F * tet)
{ return false; }

bool Geometry::intersectRay(unsigned icomponent, const Ray * r,
					Vector3F & hitP, Vector3F & hitN, float & hitDistance)
{ return false; }

void Geometry::closestToPoint(ClosestToPointTestResult * result) {}

void Geometry::closestToPointElms(const std::vector<unsigned > & elements, ClosestToPointTestResult * result)
{ 
	std::vector<unsigned >::const_iterator it = elements.begin();
	for(;it!=elements.end();++it) closestToPoint(*it, result);
}

void Geometry::closestToPoint(unsigned icomponent, ClosestToPointTestResult * result) 
{}

const Vector3F Geometry::boundingCenter() const
{
	BoundingBox box = calculateBBox();
	return box.center();
}

bool Geometry::intersectSphere(unsigned icomponent, const gjk::Sphere & B)
{ return false; }

}
//:~