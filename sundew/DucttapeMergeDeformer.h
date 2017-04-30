/*
 *  DucttapeMergeDeformer.h
 *  manuka
 *
 *  Created by jian zhang on 1/22/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include <maya/MPxDeformerNode.h>
#include <maya/MTypeId.h> 
#include <maya/MIntArray.h>
#include <maya/MPointArray.h>

class DucttapeMergeDeformer : public MPxDeformerNode
{
public:
						DucttapeMergeDeformer();
	virtual				~DucttapeMergeDeformer();

	static  void*		creator();
	static  MStatus		initialize();

    virtual MStatus   	deform(MDataBlock& 		block,
							   MItGeometry& 	iter,
							   const MMatrix& 	mat,
							   unsigned int 	multiIndex);
	virtual MStatus connectionMade ( const MPlug & plug, const MPlug & otherPlug, bool asSrc );
	virtual MStatus connectionBroken ( const MPlug & plug, const MPlug & otherPlug, bool asSrc );
	
	static MTypeId		id;
	static MObject ainmesh;

private:
	MItGeometry * m_inputGeomIter;
};
