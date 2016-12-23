#include "HesperisPolygonalMeshIO.h"
#include <maya/MFnMesh.h>
#include <maya/MGlobal.h>
#include <maya/MPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MItMeshPolygon.h>
#include <geom/APolygonalMesh.h>
#include <geom/APolygonalUV.h>
#include "HesperisFile.h"
#include <HWorld.h>
#include <foundation/SHelper.h>
#include <HTransform.h>
#include <HPolygonalMesh.h>
#include <AHelper.h>
#include <baseUtil.h>

namespace aphid {
    
bool HesperisPolygonalMeshIO::WritePolygonalMeshes(const MDagPathArray & paths, HesperisFile * file)
{
    file->clearPolygonalMeshes();
	const unsigned n = paths.length();
	unsigned i;
    for(i=0; i<n; i++) {
        APolygonalMesh * mesh = new APolygonalMesh;
        CreateMeshData(mesh, paths[i]);
        // MGlobal::displayInfo(mesh->verbosestr().c_str());
        file->addPolygonalMesh( H5PathNameTo(paths[i]), mesh );
    }
    
    file->setDirty();
	file->setWriteComponent(HesperisFile::WPoly);
	bool fstat = file->save();
	if(!fstat) MGlobal::displayWarning(MString(" cannot save poly mesh to file ")+ file->fileName().c_str());
	file->close();
    
    return true;
}

bool HesperisPolygonalMeshIO::CreateMeshData(APolygonalMesh * data, const MDagPath & path)
{
    MStatus stat;
    MFnMesh fmesh(path.node(), &stat);
    if(!stat) {
        // MGlobal::displayInfo(MString(" not a mesh ") + path.fullPathName());
        return false;
    }
	
    unsigned np = fmesh.numVertices();
    unsigned nf = fmesh.numPolygons();
    unsigned ni = fmesh.numFaceVertices();
    
    data->create(np, ni, nf);
    Vector3F * pnts = data->points();
	unsigned * inds = data->indices();
    unsigned * cnts = data->faceCounts();
    
    MPointArray ps;
    MPoint wp;
	MMatrix worldTm = AHelper::GetWorldTransformMatrix(path);
    fmesh.getPoints(ps, MSpace::kObject);
	
    unsigned i = 0;
    for(;i<np;i++) {
        wp  = ps[i] * worldTm - GlobalReferencePoint;
        pnts[i].set((float)wp.x, (float)wp.y, (float)wp.z);
    }
    
    unsigned j;
    unsigned acc = 0;
    MIntArray vertices;
    MItMeshPolygon faceIt(path);
    for(i=0; !faceIt.isDone(); faceIt.next(), i++) {
        cnts[i] = faceIt.polygonVertexCount();
        faceIt.getVertices(vertices);
        for(j = 0; j < vertices.length(); j++) {
            inds[acc] = vertices[j];
            acc++;
        }
    }
    
    data->computeFaceDrift();
	
	if(fmesh.numUVSets() < 1) {
		MGlobal::displayWarning(MString(" mesh has no uv ")+path.fullPathName());
		return true;
	}
	
	MStringArray setNames;
	fmesh.getUVSetNames(setNames);
	
	for(i=0; i< setNames.length(); i++) {
		APolygonalUV * auv = new APolygonalUV;
		CreateMeshUV(auv, path, setNames[i]);
		data->addUV(setNames[i].asChar(), auv);
	}
	
    return true;
}

bool HesperisPolygonalMeshIO::CreateMeshUV(APolygonalUV * data, const MDagPath & path, const MString & setName)
{
	MFloatArray uarray, varray;
    MIntArray uvIds;
	
	MFnMesh fmesh(path.node());
	fmesh.getUVs( uarray, varray, &setName );
	
	MItMeshPolygon faceIt(path);
	for( ; !faceIt.isDone(); faceIt.next() ) {
        for( int k=0; k < faceIt.polygonVertexCount(); k++ ) {
            int aid;
            faceIt.getUVIndex( k, aid, &setName );
            uvIds.append(aid);
        }
    }
	
	unsigned ncoords = uarray.length();
	unsigned ninds = uvIds.length();
	
	data->create(ncoords, ninds);
	
	float * u = data->ucoord();
	float * v = data->vcoord();
	unsigned * ind = data->indices();
	
	unsigned i;
	for(i=0; i< ncoords; i++) {
		u[i] = uarray[i];
		v[i] = varray[i];
	}
	
	for(i=0; i< ninds; i++) 
		ind[i] = uvIds[i];

	return true;
}

bool HesperisPolygonalMeshIO::ReadMeshes(MObject &target)
{
    MGlobal::displayInfo("opium v3 read poly mesh");
    HWorld grpWorld;
    ReadTransformAnd<HPolygonalMesh, APolygonalMesh, HesperisPolygonalMeshCreator>(&grpWorld, target);
    grpWorld.close();
    return true;
}

bool HesperisPolygonalMeshIO::ConnectUv(MObject &target)
{
    MGlobal::displayInfo("opium v3 connect poly mesh uv");
    
    MStatus stat;
    MDGModifier dgModifier;
    MObject ohes = dgModifier.createNode("hesMesh", &stat)	;
    dgModifier.doIt();
    
    if(ohes.isNull() || !stat) {
        MGlobal::displayInfo(" cannot create hes node");
        return false;
    }
    
    AHelper::Info<MString>(" hesMesh node is ", MFnDependencyNode(ohes).name() );
    HesperisMeshUvConnector::MasterMeshNode = ohes;

	HWorld grpWorld;
    ReadTransformAnd<HPolygonalMesh, APolygonalMesh, HesperisMeshUvConnector>(&grpWorld, target);
    grpWorld.close();
    return true;
}

MObject HesperisPolygonalMeshCreator::create(APolygonalMesh * data, MObject & parentObj,
                       const std::string & nodeName)
{
	const int numVertices = data->numPoints();
    MObject otm = MObject::kNullObj;
    if(nodeName.size() > 0) {
    if(HesperisIO::FindNamedChild(otm, nodeName, parentObj)) {
        if(!checkMeshNv(otm, numVertices))
			MGlobal::displayWarning(MString(" existing node ")+ nodeName.c_str() + 
				MString(" is not a mesh or it is a mesh has wrong number of cvs"));
/// checked and do not create
		return otm;
    }
    }

    MPointArray vertexArray;
	MIntArray polygonCounts, polygonConnects;
	const int numPolygons = data->numPolygons();
    
    Vector3F * cvs = data->points();
    int i = 0;
    for(;i<numVertices;i++)
        vertexArray.append( MPoint( cvs[i].x, cvs[i].y, cvs[i].z ) );
    
    unsigned * counts = data->faceCounts();
    for(i=0;i<numPolygons;i++)
        polygonCounts.append(counts[i]);
    
    const int numFaceVertices = data->numIndices();
    unsigned * conns = data->indices();
    for(i=0;i<numFaceVertices;i++)
        polygonConnects.append(conns[i]);
    
    MStatus stat;
    MFnMesh fmesh;
	otm = fmesh.create(numVertices, numPolygons, vertexArray, polygonCounts, polygonConnects, parentObj, &stat );

	if(!stat) {
		AHelper::Info<std::string>(" hesperis failed to create poly mesh ", nodeName);
		return otm;
	}
	
    if(nodeName.size() > 0) {
	std::string validName(nodeName);
	SHelper::noColon(validName);
	fmesh.setName(validName.c_str()); 
	// AHelper::Info<std::string>("create poly", validName);
    }
	
	if(data->numUVs() < 1) {
		MGlobal::displayWarning(MString(" poly mesh has no uv ")+nodeName.c_str());
		return otm;
	}

	if(data->numUVs() > 1) {
        MGlobal::displayWarning(MString(" poly mesh has multiple uv ")+nodeName.c_str());
    }
    
	for(i=0;i<data->numUVs();i++) {
		std::string setName = data->uvName(i);
/// default uv set
		if(setName == "map1") addUV(data->uvData(setName), otm, setName, polygonCounts);
	}
		
    return otm;
}

void HesperisPolygonalMeshCreator::addUV(APolygonalUV * data, MObject & mesh,
						const std::string & setName,
						const MIntArray & uvCounts)
{
	MFloatArray uArray, vArray;
	MIntArray uvIds;
	const unsigned ncoord = data->numCoords();
	const unsigned nind = data->numIndices();
	
	float * u = data->ucoord();
	float * v = data->vcoord(); 
	unsigned i = 0;
	for(;i<ncoord;i++) {
		uArray.append(u[i]);
		vArray.append(v[i]);
	}
	
	unsigned * ind = data->indices();
	for(i=0; i<nind; i++) {
		uvIds.append(ind[i]);
    }
    	
	MString uvSet(setName.c_str());

    MStatus stat;
    MFnMesh fmesh(mesh, &stat);
    if(!stat) {
		MGlobal::displayInfo(" hesperis cannot create poly mesh fn to add uv");
		return;
	}
    
    stat = fmesh.setUVs( uArray, vArray, &uvSet);
	if(!stat)
		MGlobal::displayWarning(MString(" hesperis failed to create uv set coord ")+uvSet);
	
	stat = fmesh.assignUVs( uvCounts, uvIds, &uvSet );
	if(!stat)
		MGlobal::displayWarning(MString(" hesperis failed to create uv set uvid ")+uvSet);
		
	return;
}

bool HesperisPolygonalMeshCreator::checkMeshNv(const MObject & node, unsigned nv)
{
	MStatus stat;
	MFnMesh fmesh(node, &stat);
	if(!stat)
		return false;
	
	return (fmesh.numVertices() == nv);
}

MObject HesperisMeshUvConnector::MasterMeshNode;

MObject HesperisMeshUvConnector::create(APolygonalMesh * data, MObject & parentObj,
                       const std::string & nodeName)
{
    const int numVertices = data->numPoints();
    
    MObject otm = MObject::kNullObj;
    if(HesperisIO::FindNamedChild(otm, nodeName, parentObj)) {
        if( !HesperisPolygonalMeshCreator::checkMeshNv(otm, numVertices)) {
			MGlobal::displayWarning(MString(" existing node ")+ nodeName.c_str() + 
				MString(" is not a mesh or it is a mesh has wrong number of cvs"));
            return otm;
        }
    }
    if(otm.isNull()) {
        AHelper::Info<std::string>(" no existing node to add uv ", nodeName);
        return otm;
    }
    
	MFnDependencyNode fnmaster(MasterMeshNode);
    MStatus stat;
    MPlug hesNamePlug = fnmaster.findPlug("hesPath", false, &stat );
    if(!stat) {
        MGlobal::displayWarning(" cannot find hes file name");
        return otm;
    }
    
    hesNamePlug.setValue(MString(BaseUtil::HesDoc->fileName().c_str() ) );
    
    MPlug meshNamePlugs = fnmaster.findPlug("meshName", false, &stat );
    if(!stat) {
        MGlobal::displayWarning(" cannot find hes mesh out");
        return otm;
    }
    
/// add to last
    unsigned count = meshNamePlugs.numElements();
/// AHelper::Info<unsigned>(" hes has n mesh ", count);
    meshNamePlugs.selectAncestorLogicalIndex(count);
    meshNamePlugs.setValue(MString(HesperisIO::CurrentHObjectPath.c_str() ) );

    MPlug outMeshArrayPlug = fnmaster.findPlug("outMesh", true, &stat );
    MPlug outMeshPlug = outMeshArrayPlug.elementByLogicalIndex(count);
    
    // AHelper::Info<MString>("hes mesh uv out ", outMeshPlug.name() );

    MPlug inMeshPlug = MFnDependencyNode(otm).findPlug("inMesh");
    // AHelper::Info<MString>("hes mesh uv in ", inMeshPlug.name() );

    MDGModifier dgModifier;
    if(data->numUVs() < 2) {
/// single uv set
        dgModifier.connect(outMeshPlug, inMeshPlug);
        dgModifier.doIt();
    }
    else {
		MPlug creatUVPlug = createUVSets(data, outMeshPlug, dgModifier);
		
        MObject uv = dgModifier.createNode("hesMeshUV");
        dgModifier.doIt();
        
        MFnDependencyNode fuv(uv);
        MPlug uvHesNamePlug = fuv.findPlug("hesPath");
        uvHesNamePlug.setValue(MString(BaseUtil::HesDoc->fileName().c_str() ) );
        
        MPlug uvMeshNamePlug = fuv.findPlug("meshName");
        uvMeshNamePlug.setValue(MString(HesperisIO::CurrentHObjectPath.c_str() ) );
        
        MPlug uvInMeshPlug = fuv.findPlug("inMesh");
        dgModifier.connect(creatUVPlug, uvInMeshPlug);
        dgModifier.doIt();
        
        MPlug uvOutMeshPlug = fuv.findPlug("outMesh");
        dgModifier.connect(uvOutMeshPlug, inMeshPlug);
        dgModifier.doIt();
    }

    return otm;
}

void HesperisMeshUvConnector::appendUV(APolygonalMesh * data, MObject & parentObj)
{
	if(data->numUVs() < 2) {
		MGlobal::displayInfo(" hesperis has no uv to append");
		return;
	}
	
	MIntArray polygonCounts;
	const int numPolygons = data->numPolygons();
    
    unsigned * counts = data->faceCounts();
	int i;
    for(i=0;i<numPolygons;i++)
        polygonCounts.append(counts[i]);
    
	for(i=0;i<data->numUVs();i++) {
		std::string setName = data->uvName(i);
		HesperisPolygonalMeshCreator::addUV(data->uvData(setName), parentObj, setName, polygonCounts);
	}
}

MPlug HesperisMeshUvConnector::createUVSets(APolygonalMesh * data, MPlug & origin, MDGModifier & modif)
{
	MPlug outMeshPlug;
	bool isFirst = true;
	int i = 0;
	for(;i<data->numUVs();i++) {
		std::string setName = data->uvName(i);
		if(setName != "map1" ) {
			MObject ouvcrt = modif.createNode("createUVSet" );
			modif.doIt();
			AHelper::Info<std::string >(" create uvset ", setName );
			MFnDependencyNode fuvcrt(ouvcrt);
			fuvcrt.findPlug ( MString("uvSetName") ).setValue(MString(setName.c_str() ) );
			
			MPlug inMeshPlug = fuvcrt.findPlug("inputGeometry");
				
			if(isFirst) {
				isFirst = false;
				modif.connect(origin , inMeshPlug);
			}
			else {
				modif.connect(outMeshPlug, inMeshPlug);
			}
			modif.doIt();
			
			outMeshPlug = fuvcrt.findPlug("outputGeometry");
		}
	}
	
	return outMeshPlug;
}

}
//:~
