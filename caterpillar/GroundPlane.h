/*
 *  GroundPlane.h
 *  caterpillar
 *
 *  Created by jian zhang on 3/1/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <maya/MPxLocatorNode.h> 
#include <maya/MTypeId.h> 
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/M3dView.h>
#include <maya/MTime.h>
#include <maya/MGlobal.h>
#include <maya/MDagPath.h>
#include "Ground.h"

namespace caterpillar {
class GroundPlane : public MPxLocatorNode, public Ground
{
public:
	GroundPlane();
	virtual ~GroundPlane(); 

    virtual MStatus   		compute( const MPlug& plug, MDataBlock& data );

	virtual void            draw( M3dView & view, const MDagPath & path, 
								  M3dView::DisplayStyle style,
								  M3dView::DisplayStatus status );

	virtual bool            isBounded() const;
	virtual MBoundingBox    boundingBox() const; 

	static  void *          creator();
	static  MStatus         initialize();

	static  MObject a_outSolver;
	static  MObject a_inTime;
	static  MObject a_inMesh;
	static MObject a_inFriction;
	static MObject a_inMargin;
	static	MTypeId id;
	
private:
	void computeCreate(MDataBlock& data);
	void computeUpdate(MDataBlock& data);
};
}
