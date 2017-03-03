/*
 *  aSearchHelper.h
 *  opium
 *
 *  Created by jian zhang on 6/14/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef APH_MAMA_SEARCH_HELPER_H
#define APH_MAMA_SEARCH_HELPER_H

#include <string>
#include <vector>
#include <map>

class MString;
class MObject;
class MObjectArray;
class MFnMesh;
class MMatrix;
class MDagPath;
class MPlug;
class MDagPathArray;

namespace aphid {

class ASearchHelper {
public:
	ASearchHelper();
	
	std::string getPullPathName(MObject &node);
	char findObjDirectChildByName(MObject &parent, MObject &result, std::string &name);
	char findObjDirectChildByNameIgnoreNamespace(MObject &parent,MObject &result,std::string &name);
	char findObjWithNamespaceDirectChildByName(MObject &parent,MObject &result,std::string &name);
	char findFirstTypedChild(MDagPath &parent, MObject &result, MFn::Type type);
	char getObjByFullName(const char* name, MObject& res, MObject& root = MObject::kNullObj);
	char getObjByFullNameIgnoreNamespace(const char* name, MObject& res, MObject& root = MObject::kNullObj);
	char fuzzyGetObjByFullName(std::string &toReplace,const char* name,MObject& res,MObject& root = MObject::kNullObj);
	char getDescendedByFullNameIgnoreNamespace(MObject& root, const char* name, MObject& res);
	char fuzzyGetDescendedByTerminalName(MObject& root, const char* name, MObject& res);
	char findTypedNodeInHistory(MObject &root, const char *nodename, MObject &res, bool downstream=false);
	char findNamedPlugInHistory(MObject &root, MFn::Type type, MString &name1, MPlug &plug1);
	char isObjInDownstream(MObject &root, MObject &obj);
	char getWorldTMByFullName(const char* name, MMatrix& res);
	char getWorldTMByObj(const MObject& root, MMatrix &res);
	char isFirstNonIdentityTM(const MObject& root, MMatrix &res);
	char isParentAllIdentityTM(const MDagPath &path);
	MObject findShadingEngine(MObject &mesh);
	MObject findMaterial(MObject &mesh);
	char getFiles(MObject &root, MObjectArray &arr);
	char allTypedNodeInHistory(MObject &root, MObjectArray &arr, const char *typeName);
	char isStringInArray(const std::vector<std::string> &arr, const std::string &val);
	char isObjInArray(MObject &obj, MObjectArray &arr);
	char isPathInArray(MDagPath &path, MDagPathArray &arr);
	char findIntermediateMeshInHistory(MObject &root, MObject &res);
	char findMatchedMeshWithNamedPlugsInHistory(MObject &root, MObject &res, MString &name1, MPlug &plug1, MString &name2, MPlug &plug2);
	char findReferencedMeshInHistory(MObject &root, MObject &res);
	char findTransformWithNamedPlugValue(const MObject &root, MObject &res, MString &name, MString &value);
	char findNodeWithNamedPlugValue2(const MObject &root, MObject &res, MString &name1, MString &value1, MString &name2, MString &value2);
	char findShadingEnginesByName(std::string& name, MObjectArray& result);
	char findSkinByMesh(MObject& mesh, MObject &skin);
	char shadingEngines(MObject &mesh, MObjectArray & sgs);
	char facesAttachedToShadingEngine(MObject &mesh, MObject & sg, MDagPath & item, MObject & component);
	char dagDirectChildByName(MDagPath &parent, MDagPath &result, std::string &name);
	char dagByFullName(const char *name, MDagPath & res);
	void lastMesh(MObject & result);
	char sameParent(MObject &one, MObject &another);
	
    static bool FirstObjByAttrValInArray(MObjectArray &objarr, MString &attrname, MString &attrval, MObject &res);
	static void LsAllTypedPaths(MDagPathArray & dst, const MDagPath & root, MFn::Type typ);
	static bool FirstTypedObj(const MObject &root, MObject & dst, MFn::Type typ);
	static bool FirstConnectedTypedDepNodeByTypename(MFn::Type type, MString& name, MObject& root, MObject& node);
    static bool FristNamedPlugInHistory(MObject &root, MFn::Type type, MString &name1, MPlug &plug1);
	static bool FirstDepNodeByName(MObject& node, const MString & name, MFn::Type type);
	static void TransformsBetween(MDagPathArray & dst,
								const MDagPath & longer, const MDagPath & shorter);
	static void TransformsToWorld(std::map<std::string, MDagPath> & dst,
								const MDagPath & longer);
	static void LsAllTransformsTo(std::map<std::string, MDagPath> & dst, const MDagPathArray & tails);
	static void LsAll(std::map<std::string, MDagPath> & dst, const MDagPathArray & tails);

private:
	static void GetChildPathTo(MDagPath & dst,
				const MObject & childO,
				const MDagPath & parentP);
	
};

}
#endif