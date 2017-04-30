/*
 *  accPatch.h
 *  catmullclark
 *
 *  Created by jian zhang on 10/28/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once
#include <bezierPatch.h>
#include <PatchNeighborRec.h>

namespace aphid {

class AccStencil;
class AccPatch : public BezierPatch {
public:
	AccPatch();
	virtual ~AccPatch();
	
	void findCorners();
	virtual void evaluateContolPoints();
	
	void processCornerControlPoints(int i);
	void processEdgeControlPoints(int i);
	void processInteriorControlPoints(int i);
	
	static AccStencil* stencil;
private:
	PatchNeighborRec m_corners[4];
};

}
