/*
 *  DucttapeCmd.cpp
 *  manuka
 *
 *  Created by jian zhang on 1/22/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#include "DucttapeCmd.h"

#include <maya/MArgDatabase.h>
#include <maya/MArgList.h>
#include <maya/MPointArray.h>
#include <maya/MDagModifier.h>
#include <maya/MDGModifier.h>
#include <maya/MFnIntArrayData.h>
#include <maya/MItSelectionList.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItMeshVertex.h>
#include <maya/MDGModifier.h>
#include <maya/MFnMesh.h>
#include <AHelper.h>
#include <ASearchHelper.h>

using namespace aphid;

DucttapeCmd::DucttapeCmd()
{
	setCommandString("DucttapeCmd");
}

DucttapeCmd::~DucttapeCmd() {}

void* DucttapeCmd::creator()
{
	return new DucttapeCmd;
}

MSyntax DucttapeCmd::newSyntax()
{
	MSyntax syntax;
	return syntax;
}

MStatus DucttapeCmd::doIt(const MArgList &args)
{
	MStatus status = parseArgs(args);

	MSelectionList selFaces;
	MGlobal::getActiveSelectionList ( selFaces );
	
	MItSelectionList faceIter( selFaces );
	
	std::vector<int> counts;
	std::vector<int> connections;
	std::map<int, IndexPoint > vertices;
	std::vector<float> ucoords;
	std::vector<float> vcoords;
	int i = 0;
	for ( ; !faceIter.isDone(); faceIter.next() ) {								
        MDagPath item;			
        MObject component;		
        faceIter.getDagPath( item, component );
		addFaces(counts, connections, vertices, i<<22, 
					ucoords, vcoords,
					item, component);
		i++;
    }
	
	if(counts.size()<1) {
		AHelper::Info<int>("DucttapeCmd error zero face selected", 0);
		return status;
	}
	
	AHelper::Info<int>("DucttapeCmd extracts n faces", counts.size() );
	
	packVertices(vertices);
	MDagPath tapeMesh = buildMesh(counts, connections, vertices, ucoords, vcoords, &status);
	if(!status) return status;
	MGlobal::select (tapeMesh, MObject::kNullObj, MGlobal::kReplaceList );
	MObject branchDeformer = AHelper::CreateDeformer("ducttapeBranchDeformer");
	if(branchDeformer.isNull() ) {
		return status;
	}
	
/// convert faces to vertices
	MSelectionList selVerts;
	faceIter.reset();
	for ( ; !faceIter.isDone(); faceIter.next() ) {
		MDagPath item;			
        MObject component;		
        faceIter.getDagPath( item, component );
		convertToVert(selVerts, item, component);
	}
	
	MGlobal::setActiveSelectionList ( selVerts, MGlobal::kReplaceList );
	MObject mergeDeformer = AHelper::CreateDeformer("ducttapeMergeDeformer");
	if(mergeDeformer.isNull() ) {
		return status;
	}
	
	connectBranch(branchDeformer, mergeDeformer);
	connectMerge(tapeMesh.node(), mergeDeformer);
	return status;
}

MStatus DucttapeCmd::parseArgs(const MArgList &args)
{
	MStatus status;
	MArgDatabase argData(syntax(), args);
	
	return MStatus::kSuccess;
}

void DucttapeCmd::addFaces(std::vector<int> & counts, 
					std::vector<int> & connections,
					std::map<int, IndexPoint > & vertices,
					int vertexIndOffset,
					std::vector<float> & ucoords,
					std::vector<float> & vcoords,
					const MDagPath & mesh, MObject & faces)
{
	MStatus stat;
	MItMeshPolygon iter(mesh, faces, &stat);
	if(!stat) return;
	
	for ( ; !iter.isDone(); iter.next() ) {								
        counts.push_back(iter.polygonVertexCount() );
		
		MIntArray indices;
		iter.getVertices (indices);
		for(unsigned i=0; i< indices.length(); ++i) {
			connections.push_back(indices[i]+vertexIndOffset);
		}
		
		MPointArray pointArray;
		iter.getPoints (pointArray);
		for(unsigned i=0; i< pointArray.length(); ++i) {
			IndexPoint indp;
			indp._pnt = pointArray[i];
			vertices[indices[i]+vertexIndOffset ] = indp;
		}
		
		MFloatArray uArray, vArray;
		iter.getUVs ( uArray, vArray);
		for(unsigned i=0; i< uArray.length(); ++i) {
			ucoords.push_back(uArray[i]);
			vcoords.push_back(vArray[i]);
		}
    }
}

void DucttapeCmd::packVertices(std::map<int, IndexPoint > & vertices)
{
	int i = 0;
	std::map<int, IndexPoint >::iterator it = vertices.begin();
	for(;it!=vertices.end();++it) {
		it->second._ind = i;
		i++;
	}
}

MDagPath DucttapeCmd::buildMesh(const std::vector<int> & counts, 
					const std::vector<int> & connections,
					std::map<int, IndexPoint > & vertices,
					const std::vector<float> & ucoords,
					const std::vector<float> & vcoords,
					MStatus * stat)
{
	int numVertices = vertices.size();
	int numPolygons = counts.size();
	
	MPointArray vertexArray;
	std::map<int, IndexPoint >::const_iterator it = vertices.begin();
	for(;it!=vertices.end();++it) {
		vertexArray.append(it->second._pnt);
	}
	
	MIntArray polygonCounts;
	std::vector<int>::const_iterator countIt = counts.begin();
	for(;countIt!=counts.end();++countIt) {
		polygonCounts.append(*countIt);
	}
	
	MIntArray polygonConnects;
	std::vector<int>::const_iterator connectionIt = connections.begin();
	for(;connectionIt!=connections.end();++connectionIt) {
		polygonConnects.append(vertices[*connectionIt]._ind);
	}
	
	MFnMesh fmesh;
	MObject omesh = fmesh.create (numVertices, numPolygons, vertexArray, polygonCounts, polygonConnects, MObject::kNullObj, stat );
	
	MFnDagNode ft(omesh);
	MDagPath pm;
	ft.getPath(pm);
	pm.extendToShape();
	AHelper::Info<MString>("DucttapeCmd create mesh", pm.fullPathName() );
	
	MObject oshape = pm.node();
	MFnMesh fuv(oshape, stat);
	if(!stat) return pm;
	
	MFloatArray uArray, vArray;
	MIntArray uvIds;
	
	const unsigned ncoords = ucoords.size();
	unsigned i=0;
	for(;i<ncoords;++i) {
		uArray.append(ucoords[i]);
		vArray.append(vcoords[i]);
		uvIds.append(i);
	}
	
	*stat = fuv.setUVs(uArray, vArray);
	*stat = fuv.assignUVs( polygonCounts, uvIds );
	
	return pm;
}

void DucttapeCmd::convertToVert(MSelectionList & selVerts, 
					const MDagPath & mesh, MObject & faces)
{
	MStatus stat;
	MItMeshPolygon iter(mesh, faces, &stat);
	if(!stat) return;
	
	MItMeshVertex vertIter(mesh, MObject::kNullObj, &stat);
	if(!stat) return;
	
	int prevIndex;
	for ( ; !iter.isDone(); iter.next() ) {								
        
		MIntArray indices;
		iter.getVertices (indices);
		for(unsigned i=0; i< indices.length(); ++i) {
			vertIter.setIndex(indices[i], prevIndex);
			MObject vert = vertIter.currentItem();
			selVerts.add (mesh, vert, true);
		}
    }
}

void DucttapeCmd::connectBranch(const MObject & branch, const MObject & merge)
{
	MStatus stat;
	MFnDependencyNode fbranch(branch);
	MPlug dstPlug = fbranch.findPlug("inMesh");
	MPlug groupIdPlug = fbranch.findPlug("inGroupId");
	MFnDependencyNode fmerge(merge);
	MPlug inputPlug = fmerge.findPlug("input");
	MDGModifier mod;
	for (int i = 0; i < inputPlug.numElements (); i++) {
/// child(0) is inputGeometry
		MPlug elementPlug = inputPlug[i].child(0);
		
		MPlugArray srcPlug;
		elementPlug.connectedTo(srcPlug, true, false, & stat);
		if(stat) {
			MGlobal::displayInfo(MString("connect ")+srcPlug[0].name()+" -> "+dstPlug.elementByLogicalIndex(i).name() );
			mod.connect(srcPlug[0], dstPlug.elementByLogicalIndex(i));
			mod.doIt();
		}
		else
			MGlobal::displayInfo(MString("cannot find connect to ")+elementPlug.name());
			
/// child(1) is groupId
		elementPlug = inputPlug[i].child(1);
		elementPlug.connectedTo(srcPlug, true, false, & stat);
		if(stat) {
			MGlobal::displayInfo(MString("connect ")+srcPlug[0].name()+" -> "+groupIdPlug.elementByLogicalIndex(i).name() );
			mod.connect(srcPlug[0], groupIdPlug.elementByLogicalIndex(i));
			mod.doIt();
		}
		else
			MGlobal::displayInfo(MString("cannot find connect to ")+elementPlug.name());
	}
}

void DucttapeCmd::connectMerge(const MObject & mesh, const MObject & merge)
{
	MStatus stat;
	MFnDependencyNode fmesh(mesh);
	MPlug srcPlug = fmesh.findPlug("outMesh");
	MFnDependencyNode fmerge(merge);
	MPlug dstPlug = fmerge.findPlug("inMesh");
	MDGModifier mod;
	MGlobal::displayInfo(MString("connect ")+srcPlug.name()+" -> "+dstPlug.name() );
	mod.connect(srcPlug, dstPlug);
	mod.doIt();
}
