#ifndef BOXPAINTTOOLCMD_H
#define BOXPAINTTOOLCMD_H

/*
 *  BoxPaintToolCmd.h
 *  proxyPaint
 *
 *  Created by jian zhang on 2/1/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */
#include <maya/MString.h>
#include <maya/MGlobal.h>
#include <maya/MDagPath.h>
#include <maya/MItSelectionList.h>
#include <maya/MSelectionList.h>
#include <maya/MPxToolCommand.h> 
#include <maya/MSyntax.h>
#include <maya/MArgParser.h>
#include <maya/MArgDatabase.h>
#include <maya/MArgList.h>
#include <sdb/VectorArray.h>
#include <math/Matrix33F.h>
#include "ShrubWorks.h"
#include "ReplacerWorks.h"
#include "GardenWorks.h"

namespace aphid {

namespace cvx {

class Triangle;

}

}

class proxyPaintTool : public MPxToolCommand, public aphid::ShrubWorks, public ReplacerWorks, public GardenWorks
{
    int m_currentVoxInd;
	int m_l2VoxInd;
	aphid::Matrix33F::RotateOrder m_rotPca;
	
public:
					proxyPaintTool(); 
	virtual			~proxyPaintTool(); 
	static void*	creator();

	MStatus			doIt(const MArgList& args);
	MStatus			parseArgs(const MArgList& args);
	static MSyntax	newSyntax();
	MStatus			finalize();

private:
	MStatus connectGroundSelected();
	MStatus replaceGroundSelected(int iSlot);
	MStatus connectVoxelSelected();
	bool connectVoxToViz(MObject & voxObj, MObject & vizObj);
	MStatus saveCacheSelected();
	MStatus loadCacheSelected();
	MStatus voxelizeSelected();
	void checkOutputConnection(MObject & node, const MString & outName);
	MStatus performPCA();
	MStatus performDFT();
	void strToRotateOrder(const MString & srod);
    bool isTransformConnected(const MDagPath & transPath, 
					const MObject & vizObj,
					int & slotLgcInd,
					MPlug & worldSpacePlug);
	bool isMeshConnectedSlot(const MObject & meshObj, 
					const MObject & vizObj,
					const int & slotLgcInd);
	void connectTransform(MPlug & worldSpacePlug, 
					MObject & vizOb);
	MStatus getSelectedViz(MSelectionList & sels, 
					MObject & vizobj,
					const int & minNumSelected,
					const char * errMsg);
	void disconnectGround(const MObject & vizobj,
					int iSlot);
	bool canConnectToViz(const MString & nodeTypeName) const;
	
private:
	enum Operation {
		opUnknown = 0,
		opBeginPick = 1,
		opDoPick = 2,
		opEndPick = 3,
		opGetPick = 4,
		opConnectGround = 5,
		opSaveCache = 6,
		opLoadCache = 7,
		opVoxelize = 8,
		opConnectVoxel = 9,
		opCreateShrub = 10,
        opPrincipalComponent = 11,
		opDistanceFieldTriangulate = 12,
		opListReplacer = 13,
		opConnectReplacer = 14,
		opReplaceGround = 15,
		opImportGarden = 16
	};
	
	Operation m_operation;
	
	unsigned opt, nseg;
	int m_replaceGroundId;
	int m_dftLevel;
	float lseg;
	double m_dftScale, m_dftRound;
	MString fBlockerName, fVizName, m_cacheName, m_gardenName;
	
};
#endif        //  #ifndef BOXPAINTTOOLCMD_H

