/*
 *  ClosestToPointEngine.h
 *  
 *
 *  Created by jian zhang on 1/13/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef APH_KD_CLOSEST_TO_POINT_ENGINE_H
#define APH_KD_CLOSEST_TO_POINT_ENGINE_H

#include <kd/KdEngine.h>

namespace aphid {

template<typename T, typename Tn>
class ClosestToPointEngine : public KdEngine {

typedef KdNTree<T, Tn > TreeTyp;

	ClosestToPointTestResult m_ctx;
	TreeTyp * m_tree;
	
public:
	ClosestToPointEngine(TreeTyp * tree);
	
	bool closestTo(Vector3F & dest, 
					const Vector3F & origin);
					
	void getGeomCompContribute(int & igeom,
					int & icomp,
					float * contrib) const;

};

template<typename T, typename Tn>
ClosestToPointEngine<T, Tn>::ClosestToPointEngine(TreeTyp * tree)
{
	m_tree = tree;
}

template<typename T, typename Tn>
bool ClosestToPointEngine<T, Tn>::closestTo(Vector3F & dest, 
					const Vector3F & origin)
{
	m_ctx.reset(origin, 1e9f);
	closestToPoint(m_tree, &m_ctx);
	if(m_ctx._hasResult) {
		dest = m_ctx._hitPoint;
	} else {
		dest = origin;
	}
	return m_ctx._hasResult;
}

template<typename T, typename Tn>
void ClosestToPointEngine<T, Tn>::getGeomCompContribute(int & igeom,
					int & icomp,
					float * contrib) const
{
	igeom = m_ctx._igeometry;
	icomp = m_ctx._icomponent;
/// up to 4 for tetrahedron
	contrib[0] = m_ctx._contributes[0];
	contrib[1] = m_ctx._contributes[1];
	contrib[2] = m_ctx._contributes[2];
	contrib[3] = m_ctx._contributes[3];
}

}
#endif