/*
 *  NTreeDomain.h
 *  
 *
 *  Created by jian zhang on 7/27/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include <Boundary.h>
#include <geom/ClosestToPointTest.h>
#include <kd/KdEngine.h>
#include <geom/ConvexShape.h>

namespace aphid {

template<typename T, typename Tn>
class NTreeDomain : public Domain {

	KdNTree<T, Tn > * m_tree;
	KdEngine m_engine;
	ClosestToPointTestResult m_closestPointCtx;
	IntersectionContext m_intersectCtx;
	
public:
	NTreeDomain(KdNTree<T, Tn > * tree);
	
	virtual FunctionType functionType() const;
	virtual bool broadphaseIntersect(const BoundingBox & bx);
	virtual bool narrowphaseHexahedron(const cvx::Hexahedron & hexa);
	virtual float distanceTo(const Vector3F & pref);
	virtual float beamIntersect(const Beam & b,
							const float & splatRadius);
	
};

template<typename T, typename Tn>
NTreeDomain<T, Tn >::NTreeDomain(KdNTree<T, Tn > * tree)
{
	m_tree = tree;
	setBBox(tree->getBBox() );
}

template<typename T, typename Tn>
Domain::FunctionType NTreeDomain<T, Tn >::functionType() const
{ return Domain::fnKdTree; }

template<typename T, typename Tn>
bool NTreeDomain<T, Tn >::broadphaseIntersect(const BoundingBox & bx)
{
	if(!getBBox().intersect(bx) )
		return false;
	
	return m_engine.broadphase(m_tree, bx);
}

template<typename T, typename Tn>
float NTreeDomain<T, Tn >::distanceTo(const Vector3F & pref)
{
    m_closestPointCtx.reset(pref, distanceRange(), true );
	m_engine.closestToPoint(m_tree, &m_closestPointCtx ); 
	return m_closestPointCtx._distance;
}

template<typename T, typename Tn>
float NTreeDomain<T, Tn >::beamIntersect(const Beam & b,
							const float & splatRadius)
{ 
	m_intersectCtx.reset(b, distanceRange() * 1e-4f );
	m_intersectCtx.setSplatRadius(splatRadius);
	m_engine.beamIntersect(m_tree, &m_intersectCtx);
	if(m_intersectCtx.m_success)
		return m_intersectCtx.m_tmin / b.ray().m_tmax;
		 
	return 1e8f; 
}

template<typename T, typename Tn>
bool NTreeDomain<T, Tn >::narrowphaseHexahedron(const cvx::Hexahedron & hexa)
{
	return m_engine.narrowphase(m_tree, hexa);;
}

}
