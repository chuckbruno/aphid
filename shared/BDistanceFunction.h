/*
 *  BDistanceFunction.h
 *  
 *	distance to a number of convex shapes
 *  Created by jian zhang on 7/14/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once
#include "ConvexShape.h"
#include "ConvexDomain.h"
#include <kd/NTreeDomain.h>

namespace aphid {

class BDistanceFunction {

	std::vector<Domain *> m_domains;
	float m_shellThickness, m_splatRadius;
	
public:
	BDistanceFunction();
	virtual ~BDistanceFunction();
	
	void setShellThickness(const float & x);
	void setSplatRadius(const float & x);
	const float & shellThickness() const;
	const float & splatRadius() const;

	void addSphere(const Vector3F & p, const float & r);
	void addBox(const Vector3F & lo, const Vector3F & hi);
	
	template<typename T, typename Tn>
	void addTree(KdNTree<T, Tn > * tree)
	{ m_domains.push_back(new NTreeDomain<T, Tn>(tree) ); }
	
/// limit distance test of all domains
	void setDomainDistanceRange(const float & x);
	float calculateDistance(const Vector3F & p);
/// closest intersect v a->b, alpha of cross
	float calculateIntersection(const Vector3F & a,
								const Vector3F & b,
								const float & ra,
								const float & rb);
	
	template<typename Ts>
	bool broadphase(const Ts * a) const
	{
		BoundingBox ab = a->calculateBBox();
		//ab.expand(shellThickness() );
		
		std::vector<Domain *>::const_iterator it = m_domains.begin();
		for(;it!=m_domains.end();++it) {
			
			Domain * d = *it;
			if(d->broadphaseIntersect(ab) )
				return true;
			
		}
		return false;
	}
	
	template<typename Ts>
	bool narrowphase(const Ts * a, const float & shellThickness) const
	{
		std::vector<Domain *>::const_iterator it = m_domains.begin();
		for(;it!=m_domains.end();++it) {
			
			Domain * dm = *it;
			bool stat = false;
			if(dm->functionType() == Domain::fnSphere) {
				SphereDomain * sd = static_cast<SphereDomain *> (dm);
				stat = sd->narrowphaseIntersect <Ts> (a, shellThickness);
			}
			//if(d->narrowphaseIntersect(a, shellThickness) )
			if(stat)
				return true;
			
		}
		return false;
	}
	
	bool narrowphase(const cvx::Hexahedron & a) const;
	
protected:
	
	
private:
	void internalClear();
	
};

}