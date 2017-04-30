/*
 *  AccPatchGroup.h
 *  aphid
 *
 *  Created by jian zhang on 11/28/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once

#include <AllMath.h>

namespace aphid {

class AccPatch;
class BezierPatchHirarchy;
class IntersectionContext;
class AccPatchGroup {
public:
	AccPatchGroup();
	virtual ~AccPatchGroup();
	void createAccPatches(unsigned n);
	AccPatch* beziers() const;
	BezierPatchHirarchy * hirarchies() const;
    void recursiveBezierClosestPoint1(IntersectionContext * ctx, int level, unsigned current) const;
	void setActiveHirarchy(unsigned idx);
	
	void recursiveBezierPatch(int level, unsigned current, std::vector<Vector3F> & dst) const;	
	void setRebuildPatchHirarchy();
private:
	AccPatch* m_bezier;
	BezierPatchHirarchy * m_hirarchy;
	BezierPatchHirarchy * m_activeHirarchy;
	unsigned m_numHirarchy;
};

}