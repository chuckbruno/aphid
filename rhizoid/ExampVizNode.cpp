/*
 *  ExampVizNode.cpp
 *  proxyPaint
 *
 *  Created by jian zhang on 2/5/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#include "ExampVizNode.h"
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnPointArrayData.h>
#include <maya/MFloatVector.h>
#include <maya/MFnPluginData.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnMatrixAttribute.h>
#include <math/BoundingBox.h>
#include <ExampData.h>
#include <math/linearMath.h>
#include <geom/ConvexShape.h>
#include <sdb/VectorArray.h>
#include <AllMama.h>

MTypeId ExampViz::id( 0x95a20e );
MObject ExampViz::abboxminv;
MObject ExampViz::abboxmaxv;
MObject ExampViz::adoplen;
MObject ExampViz::adopPBuf;
MObject ExampViz::adopNBuf;
MObject ExampViz::adrawColor;
MObject ExampViz::adrawColorR;
MObject ExampViz::adrawColorG;
MObject ExampViz::adrawColorB;
MObject ExampViz::adrawDopSizeX;
MObject ExampViz::adrawDopSizeY;
MObject ExampViz::adrawDopSizeZ;
MObject ExampViz::adrawDopSize;
MObject ExampViz::aradiusMult;
MObject ExampViz::aininstspace;
MObject ExampViz::avoxactive;
MObject ExampViz::avoxvisible;
MObject ExampViz::outValue;

using namespace aphid;

ExampViz::ExampViz()
{}

ExampViz::~ExampViz() 
{}

MStatus ExampViz::compute( const MPlug& plug, MDataBlock& block )
{
	if( plug == outValue ) {
		
		MDataHandle radiusMultH = block.inputValue(aradiusMult);
		float radiusScal = radiusMultH.asFloat();
		setGeomSizeMult(radiusScal);
		
		BoundingBox bb;
		
		MDataHandle bbminH = block.inputValue(abboxminv);
		MFloatVector& vmin = bbminH.asFloatVector();
		bb.setMin(vmin.x, vmin.y, vmin.z);
		
		MDataHandle bbmaxH = block.inputValue(abboxmaxv);
		MFloatVector& vmax = bbmaxH.asFloatVector();
		bb.setMax(vmax.x, vmax.y, vmax.z);

		setGeomBox(&bb);
		
		MDataHandle drszx = block.inputValue(adrawDopSizeX);
		MDataHandle drszy = block.inputValue(adrawDopSizeY);
		MDataHandle drszz = block.inputValue(adrawDopSizeZ);
		setDopSize(drszx.asFloat(), drszy.asFloat(), drszz.asFloat() );
	
		float * diffCol = diffuseMaterialColV();
		
		MFloatVector c = block.inputValue(adrawColor).asFloatVector();
		diffCol[0] = c.x; diffCol[1] = c.y; diffCol[2] = c.z;
		
		if(!loadTriangles(block) ) {
			AHelper::Info<MString>(" ERROR ExampViz has no draw data", MFnDependencyNode(thisMObject() ).name() );
		}
		
		MFnPluginData fnPluginData;
		MStatus status;
		MObject newDataObject = fnPluginData.create(ExampData::id, &status);
		
		ExampData * pData = (ExampData *) fnPluginData.data(&status);
		
		if(pData) pData->setDesc(this);

		MDataHandle outputHandle = block.outputValue( outValue );
		outputHandle.set( pData );
		block.setClean(plug);
    }

	return MS::kSuccess;	
}

void ExampViz::draw( M3dView & view, const MDagPath & path, 
							 M3dView::DisplayStyle style,
							 M3dView::DisplayStatus status )
{
	MObject selfNode = thisMObject();
	updateGeomBox(selfNode);
	
	MPlug rPlug(selfNode, adrawColorR);
	MPlug gPlug(selfNode, adrawColorG);
	MPlug bPlug(selfNode, adrawColorB);
	
	float * diffCol = diffuseMaterialColV();
	diffCol[0] = rPlug.asFloat();
	diffCol[1] = gPlug.asFloat();
	diffCol[2] = bPlug.asFloat();
	
	MPlug szxp(selfNode, adrawDopSizeX);
	MPlug szyp(selfNode, adrawDopSizeY);
	MPlug szzp(selfNode, adrawDopSizeZ);
	setDopSize(szxp.asFloat(), szyp.asFloat(), szzp.asFloat() );
	
	bool stat = pntBufLength() > 0;
	if(!stat) {
		stat = loadTriangles(selfNode);
	}
	if(!stat) {
		AHelper::Info<MString>(" ERROR ExampViz has no draw data", MFnDependencyNode(selfNode).name() );
		return;
	}
	
	MDagPath cameraPath;
	view.getCamera(cameraPath);
	Matrix33F mf;
	AHelper::GetViewMatrix(&mf, cameraPath);
	mf *= geomSize();
    mf.glMatrix(m_transBuf);
	
	view.beginGL();
	
	const BoundingBox & bbox = geomBox();
	drawBoundingBox(&bbox);
	
	drawZCircle(m_transBuf);
	
	glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);

	glColor3fv(diffCol);
	
	glPointSize(2.f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnableClientState(GL_VERTEX_ARRAY);
	
	drawAWireDop();
	
	glDisable(GL_LIGHTING);
	//if ( style == M3dView::kFlatShaded || 
	//	    style == M3dView::kGouraudShaded ) {
		
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		drawPoints();
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
	
	//} else {
	//	drawWiredPoints();
	//} 
	glDisableClientState(GL_VERTEX_ARRAY);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPopAttrib();
	
	view.endGL();
}

bool ExampViz::isBounded() const
{ return true; }

MBoundingBox ExampViz::boundingBox() const
{   
	const BoundingBox & bbox = geomBox();
	
	MPoint corner1(bbox.m_data[0], bbox.m_data[1], bbox.m_data[2]);
	MPoint corner2(bbox.m_data[3], bbox.m_data[4], bbox.m_data[5]);

	return MBoundingBox( corner1, corner2 );
}

void* ExampViz::creator()
{
	return new ExampViz();
}

MStatus ExampViz::initialize()
{ 
	MFnNumericAttribute numFn;
	MFnTypedAttribute typedFn;
	
	MStatus			 stat;
	
	adrawColorR = numFn.create( "dspColorR", "dspr", MFnNumericData::kFloat);
	numFn.setStorable(true);
	numFn.setKeyable(true);
	numFn.setDefault(0.47f);
	addAttribute(adrawColorR);
	
	adrawColorG = numFn.create( "dspColorG", "dspg", MFnNumericData::kFloat);
	numFn.setStorable(true);
	numFn.setKeyable(true);
	numFn.setDefault(0.46f);
	addAttribute(adrawColorG);
	
	adrawColorB = numFn.create( "dspColorB", "dspb", MFnNumericData::kFloat);
	numFn.setStorable(true);
	numFn.setKeyable(true);
	numFn.setDefault(0.45f);
	addAttribute(adrawColorB);
	
	adrawColor = numFn.create( "dspColor", "dspc", adrawColorR, adrawColorG, adrawColorB );
	numFn.setStorable(true);
	numFn.setKeyable(true);
	numFn.setUsedAsColor(true);
	numFn.setDefault(0.47f, 0.46f, 0.45f);
	addAttribute(adrawColor);
	
	adrawDopSizeX = numFn.create( "dspDopX", "ddpx", MFnNumericData::kFloat);
	numFn.setStorable(true);
	numFn.setKeyable(true);
	numFn.setDefault(0.9f);
	numFn.setMin(0.1f);
	numFn.setMax(1.f);
	addAttribute(adrawDopSizeX);
	
	adrawDopSizeY = numFn.create( "dspDopY", "ddpy", MFnNumericData::kFloat);
	numFn.setStorable(true);
	numFn.setKeyable(true);
	numFn.setDefault(0.9f);
	numFn.setMin(0.1f);
	numFn.setMax(1.f);
	addAttribute(adrawDopSizeY);
	
	adrawDopSizeZ = numFn.create( "dspDopZ", "ddpz", MFnNumericData::kFloat);
	numFn.setStorable(true);
	numFn.setKeyable(true);
	numFn.setDefault(0.9f);
	numFn.setMin(0.1f);
	numFn.setMax(1.f);
	addAttribute(adrawDopSizeZ);
	
	adrawDopSize = numFn.create( "dspDop", "ddps", adrawDopSizeX, adrawDopSizeY, adrawDopSizeZ );
	numFn.setStorable(true);
	numFn.setKeyable(true);
	numFn.setUsedAsColor(true);
	numFn.setDefault(0.9f, 0.9f, 0.9f);
	addAttribute(adrawDopSize);
	
	aradiusMult = numFn.create( "radiusMultiplier", "rml", MFnNumericData::kFloat);
	numFn.setStorable(true);
	numFn.setKeyable(true);
	numFn.setDefault(1.f);
	numFn.setMin(.05f);
	addAttribute(aradiusMult);
	
	avoxactive = numFn.create( "exampleActive", "exa", MFnNumericData::kBoolean);
	numFn.setStorable(true);
	numFn.setDefault(true);
	addAttribute(avoxactive);
	
	avoxvisible = numFn.create( "exampleVisible", "exv", MFnNumericData::kBoolean);
	numFn.setStorable(true);
	numFn.setDefault(true);
	addAttribute(avoxvisible);
	
	abboxminv = numFn.create( "BBoxMin", "bbxmn", MFnNumericData::k3Float );
	numFn.setStorable(true);
	numFn.setDefault(-1.f, -1.f, -1.f);
	addAttribute(abboxminv);
	
	abboxmaxv = numFn.create( "BBoxMax", "bbxmx", MFnNumericData::k3Float );
	numFn.setStorable(true);
	numFn.setDefault(1.f, 1.f, 1.f);
	addAttribute(abboxmaxv);
	
	outValue = typedFn.create( "outValue", "ov", MFnData::kPlugin );
	typedFn.setStorable(false);
	typedFn.setWritable(false);
	addAttribute(outValue);
	
	MPointArray defaultPntArray;
	MFnPointArrayData pntArrayDataFn;
	pntArrayDataFn.create( defaultPntArray );
	
	adoplen = numFn.create( "dopLen", "dpl", MFnNumericData::kInt, 0 );
	numFn.setStorable(true);
	addAttribute(adoplen);
	
	MVectorArray defaultVecArray;
	MFnVectorArrayData vecArrayDataFn;
	vecArrayDataFn.create( defaultVecArray );
	adopPBuf = typedFn.create( "dopPBuf", "dpp",
											MFnData::kVectorArray,
											vecArrayDataFn.object(),
											&stat );
    typedFn.setStorable(true);
	addAttribute(adopPBuf);
	
	adopNBuf = typedFn.create( "dopNBuf", "dpn",
											MFnData::kVectorArray,
											vecArrayDataFn.object(),
											&stat );
    typedFn.setStorable(true);
	addAttribute(adopNBuf);
	
	MFnMatrixAttribute matAttr;
	aininstspace = matAttr.create("instanceSpace", "sinst", MFnMatrixAttribute::kDouble);
	matAttr.setStorable(false);
	matAttr.setWritable(true);
	matAttr.setConnectable(true);
    matAttr.setArray(true);
    matAttr.setDisconnectBehavior(MFnAttribute::kDelete);
	addAttribute( aininstspace );
	
	attributeAffects(aradiusMult, outValue);
	attributeAffects(adoplen, outValue);
	attributeAffects(adopPBuf, outValue);
	attributeAffects(adopNBuf, outValue);
	attributeAffects(adrawColorR, outValue);
	attributeAffects(adrawColorG, outValue);
	attributeAffects(adrawColorB, outValue);
	attributeAffects(adrawDopSizeX, outValue);
	attributeAffects(adrawDopSizeY, outValue);
	attributeAffects(adrawDopSizeZ, outValue);
	attributeAffects(avoxactive, outValue);
	attributeAffects(avoxvisible, outValue);
	return MS::kSuccess;
}

MStatus ExampViz::connectionMade ( const MPlug & plug, const MPlug & otherPlug, bool asSrc )
{
	if(plug == outValue)
		AHelper::Info<MString>("connect", plug.name());
	return MPxLocatorNode::connectionMade (plug, otherPlug, asSrc );
}

MStatus ExampViz::connectionBroken ( const MPlug & plug, const MPlug & otherPlug, bool asSrc )
{
	if(plug == outValue)
		AHelper::Info<MString>("disconnect", plug.name());
	return MPxLocatorNode::connectionMade (plug, otherPlug, asSrc );
}

void ExampViz::voxelize2(sdb::VectorArray<cvx::Triangle> * tri,
							const BoundingBox & bbox)
{
	ExampVox::voxelize2(tri, bbox);
	
	MFnNumericData bbFn;
	MObject bbData = bbFn.create(MFnNumericData::k3Float);
	
	bbFn.setData(bbox.data()[0], bbox.data()[1], bbox.data()[2]);
	MPlug bbmnPlug(thisMObject(), abboxminv);
	bbmnPlug.setValue(bbData);
	
	bbFn.setData(bbox.data()[3], bbox.data()[4], bbox.data()[5]);
	MPlug bbmxPlug(thisMObject(), abboxmaxv);
	bbmxPlug.setValue(bbData);
	
	const int n = pntBufLength();
	MPlug dopLenPlug(thisMObject(), adoplen);
	dopLenPlug.setInt(n);
	if(n < 1) return;
	
	MVectorArray dopp; dopp.setLength(n);
	MVectorArray dopn; dopn.setLength(n);
	const Vector3F * ps = pntPositionR();
	const Vector3F * ns = pntNormalR();
	for(int i=0; i<n; ++i) {
		dopp[i] = MVector(ps[i].x, ps[i].y, ps[i].z);
		dopn[i] = MVector(ns[i].x, ns[i].y, ns[i].z);
	}
	
	MFnVectorArrayData vecFn;
	MObject opnt = vecFn.create(dopp);
	MPlug doppPlug(thisMObject(), adopPBuf);
	doppPlug.setValue(opnt);
	
	MObject onor = vecFn.create(dopn);
	MPlug dopnPlug(thisMObject(), adopNBuf);
	dopnPlug.setValue(onor);
	
	AHelper::Info<int>("reduced draw n point ", pntBufLength() );
}

void ExampViz::voxelize3(sdb::VectorArray<cvx::Triangle> * tri,
							const BoundingBox & bbox)
{
	ExampVox::voxelize3(tri, bbox);
	
	MFnNumericData bbFn;
	MObject bbData = bbFn.create(MFnNumericData::k3Float);
	
	bbFn.setData(bbox.data()[0], bbox.data()[1], bbox.data()[2]);
	MPlug bbmnPlug(thisMObject(), abboxminv);
	bbmnPlug.setValue(bbData);
	
	bbFn.setData(bbox.data()[3], bbox.data()[4], bbox.data()[5]);
	MPlug bbmxPlug(thisMObject(), abboxmaxv);
	bbmxPlug.setValue(bbData);
	
	const int n = pntBufLength();
	MPlug dopLenPlug(thisMObject(), adoplen);
	dopLenPlug.setInt(n);
	if(n < 1) {
		return;
	}
	
	MVectorArray dopp; dopp.setLength(n);
	MVectorArray dopn; dopn.setLength(n);
	const Vector3F * ps = pntPositionR();
	const Vector3F * ns = pntNormalR();
	for(int i=0; i<n; ++i) {
		dopp[i] = MVector(ps[i].x, ps[i].y, ps[i].z);
		dopn[i] = MVector(ns[i].x, ns[i].y, ns[i].z);
	}
	
	MFnVectorArrayData vecFn;
	MObject opnt = vecFn.create(dopp);
	MPlug doppPlug(thisMObject(), adopPBuf);
	doppPlug.setValue(opnt);
	
	MObject onor = vecFn.create(dopn);
	MPlug dopnPlug(thisMObject(), adopNBuf);
	dopnPlug.setValue(onor);
	
	AHelper::Info<int>("reduced draw n point ", pntBufLength() );
}

void ExampViz::voxelize3(const aphid::DenseMatrix<float> & pnts,
						const MIntArray & triangleVertices,
						const aphid::BoundingBox & bbox)
{
	const int nind = triangleVertices.length();
	const int ntri = nind / 3;
	const float * vps = pnts.column(0);
						
	sdb::VectorArray<cvx::Triangle> tris;
	
	for(int i=0;i<ntri;++i) {
		aphid::cvx::Triangle atri;
		
		for(int j=0;j<3;++j) {
			const float * ci = &vps[triangleVertices[i*3+j] * 3];
			Vector3F fp(ci[0], ci[1], ci[2]);
			atri.setP(fp, j);
		}
		
		tris.insert(atri);
	}
	
	voxelize3(&tris, bbox);
}

void ExampViz::updateGeomBox(MObject & node)
{
	MPlug radiusMultPlug(node, aradiusMult);
	float radiusScal = radiusMultPlug.asFloat();
	setGeomSizeMult(radiusScal);
	
	BoundingBox bb;
	
	MObject bbmn;
	MPlug bbmnPlug(node, abboxminv);
	bbmnPlug.getValue(bbmn);
	MFnNumericData bbmnFn(bbmn);
	bbmnFn.getData3Float(bb.m_data[0], bb.m_data[1], bb.m_data[2]);
	
	MObject bbmx;
	MPlug bbmxPlug(node, abboxmaxv);
	bbmxPlug.getValue(bbmx);
	MFnNumericData bbmxFn(bbmx);
	bbmxFn.getData3Float(bb.m_data[3], bb.m_data[4], bb.m_data[5]);
	
	setGeomBox(&bb);
				
}

bool ExampViz::loadTriangles(MDataBlock & data)
{
	int n = data.inputValue(adoplen).asInt();
	if(n < 1) {
		AHelper::Info<int>(" ExampViz error zero n triangle", n);
		return false;
	}
	
	MDataHandle pntH = data.inputValue(adopPBuf);
	MFnVectorArrayData pntFn(pntH.data());
	MVectorArray pnts = pntFn.array();
	
	if(pnts.length() < n) {
		AHelper::Info<unsigned>(" ExampViz error wrong triangle position length", pnts.length() );
		return false;
	}
	
	MDataHandle norH = data.inputValue(adopNBuf);
	MFnVectorArrayData norFn(norH.data());
	MVectorArray nors = norFn.array();
	
	if(nors.length() < n) {
		AHelper::Info<unsigned>(" ExampViz error wrong triangle normal length", pnts.length() );
		return false;
	}
	
	setPointDrawBufLen(n);
	
	Vector3F * ps = pntPositionR();
	Vector3F * ns = pntNormalR();
	for(int i=0; i<n; ++i) {
		ps[i].set(pnts[i].x, pnts[i].y, pnts[i].z);
		ns[i].set(nors[i].x, nors[i].y, nors[i].z);
	}
	
	AHelper::Info<int>(" ExampViz load n point", n );
	buildBounding8Dop(geomBox() );
	return true;
}

bool ExampViz::loadTriangles(MObject & node)
{
	MPlug doplenPlug(node, adoplen);
	int n = doplenPlug.asInt();
	if(n<1) return false;
	
	MPlug doppPlug(node, adopPBuf);
	MObject doppObj;
	doppPlug.getValue(doppObj);
	
	MFnVectorArrayData pntFn(doppObj);
	MVectorArray pnts = pntFn.array();
	
	unsigned np = pnts.length();
	if(np < n ) {
		AHelper::Info<unsigned>(" ExampViz error wrong triangle position length", np );
		return false;
	}
	
	MPlug dopnPlug(node, adopNBuf);
	MObject dopnObj;
	dopnPlug.getValue(dopnObj);
	
	MFnVectorArrayData norFn(dopnObj);
	MVectorArray nors = norFn.array();
	
	unsigned nn = nors.length();
	if(nn < n ) {
		AHelper::Info<unsigned>(" ExampViz error wrong triangle normal length", nn );
		return false;
	}

	setPointDrawBufLen(n);
	
	Vector3F * ps = pntPositionR();
	Vector3F * ns = pntNormalR();
	for(int i=0; i<n; ++i) {
		ps[i].set(pnts[i].x, pnts[i].y, pnts[i].z);
		ns[i].set(nors[i].x, nors[i].y, nors[i].z);
	}
	
	AHelper::Info<unsigned>(" ExampViz load n point", n );
	buildBounding8Dop(geomBox() );
	return true;
}
//:~