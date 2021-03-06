/*
 *  AttributeHelper.cpp
 *  opium
 *
 *  Created by jian zhang on 9/19/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "AttributeHelper.h"
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnAnimCurve.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MPlugArray.h>

namespace aphid {

AttributeHelper::AttributeHelper()
{}

void AttributeHelper::getColorAttributeByName(const MFnDependencyNode& fnode, const char* attrname, double& r, double& g, double& b)
{
	MPlug plgR = fnode.findPlug(MString(attrname)+"R");
	plgR.getValue(r);
	
	MPlug plgG = fnode.findPlug(MString(attrname)+"G");
	plgG.getValue(g);
	
	MPlug plgB = fnode.findPlug(MString(attrname)+"B");
	plgB.getValue(b);
}

void AttributeHelper::getNormalAttributeByName(const MFnDependencyNode& fnode, const char* attrname, double& r, double& g, double& b)
{
	MPlug plgR = fnode.findPlug(MString(attrname)+"X");
	plgR.getValue(r);
	
	MPlug plgG = fnode.findPlug(MString(attrname)+"Y");
	plgG.getValue(g);
	
	MPlug plgB = fnode.findPlug(MString(attrname)+"Z");
	plgB.getValue(b);
}

char AttributeHelper::getDoubleAttributeByName(const MFnDependencyNode& fnode, const char* attrname, double& v)
{
	MPlug plgV = fnode.findPlug(MString(attrname));
	if(plgV.isNull()) return 0;
	plgV.getValue(v);
	return 1;
}

char AttributeHelper::getBoolAttributeByName(const MFnDependencyNode& fnode, const char* attrname, bool& v)
{
	MPlug plgV = fnode.findPlug(MString(attrname));
	if(plgV.isNull()) return 0;
	plgV.getValue(v);
	return 1;
}

char AttributeHelper::getDoubleAttributeByNameAndTime(const MFnDependencyNode& fnode, const char* attrname, MDGContext & ctx, double& v)
{
	MPlug plgV = fnode.findPlug(MString(attrname));
	if(plgV.isNull()) return 0;
	plgV.getValue(v, ctx);
	return 1;
}

char AttributeHelper::getIntAttributeByName(const MFnDependencyNode& fnode, const char* attrname, int& v)
{
	MPlug plgV = fnode.findPlug(MString(attrname));
	if(plgV.isNull()) return 0;
	plgV.getValue(v);
	return 1;
}

char AttributeHelper::getStringAttributeByName(const MFnDependencyNode& fnode, const char* attrname, MString& v)
{
	MPlug plgV = fnode.findPlug(MString(attrname));
	if(plgV.isNull()) return 0;
	plgV.getValue(v);
	return 1;
}

char AttributeHelper::getStringAttributeByName(const MObject& node, const char* attrname, MString& v)
{
	MFnDependencyNode fnode(node);
	MPlug plgV = fnode.findPlug(MString(attrname));
	if(plgV.isNull()) return 0;
	plgV.getValue(v);
	return 1;
}

void AttributeHelper::setCustomStringAttrib(MObject node, const char* nameLong, const char* nameShort, const char* value)
{
	MFnDependencyNode fdep(node);
	MStatus stat;
	if(!fdep.hasAttribute(nameLong, &stat))
	{
		MFnTypedAttribute strAttr;
		MObject attribObject = strAttr.create(nameLong, nameShort, MFnData::kString);
		strAttr.setStorable(true);
		fdep.addAttribute( attribObject, MFnDependencyNode::kLocalDynamicAttr ); 
	}
	MPlug attribPlug = fdep.findPlug (nameLong, &stat);
	attribPlug.setLocked(0);
	attribPlug.setValue( value );
	attribPlug.setLocked(1);
}

AAttribute::AttributeType AttributeHelper::GetAttribType(const MObject & entity)
{
	if(entity.isNull()) {
        return AAttribute::aUnknown;
    }
    if(entity.hasFn(MFn::kEnumAttribute)) {
        return AAttribute::aEnum;
    }
    if(entity.hasFn(MFn::kCompoundAttribute)) {
        return AAttribute::aCompound;
    } 
    if(entity.hasFn(MFn::kNumericAttribute)) {
        return AAttribute::aNumeric;
    }
    if(entity.hasFn(MFn::kTypedAttribute)) {
		MFnTypedAttribute fn(entity);
		if(fn.attrType() != MFnData::kString) {
			MGlobal::displayWarning(fn.name() + " is typed but not string attrib");
			return AAttribute::aUnknown;
		}
        return AAttribute::aString;
    }
	if(entity.hasFn(MFn::kUnitAttribute)) {
		return AAttribute::aUnit;
	}
    return AAttribute::aUnknown;
}

MFnNumericData::Type AttributeHelper::GetNumericAttributeType(const std::string & name)
{
	if(name == "short") return MFnNumericData::kShort;
	if(name == "int") return MFnNumericData::kInt;
	if(name == "float") return MFnNumericData::kFloat;
	if(name == "double") return MFnNumericData::kDouble;
	if(name == "boolean") return MFnNumericData::kBoolean;
	return MFnNumericData::kInvalid;
}

AAttribute::AttributeType AttributeHelper::AsAttributeType(const std::string & name)
{
    if(name == "string") return AAttribute::aString;
    if(name == "enum") return AAttribute::aEnum;
    if(name == "compound") return AAttribute::aCompound;
    return AAttribute::aNumeric;
}

AStringAttribute * AttributeHelper::AsStrData(const MPlug & plg)
{
	MFnTypedAttribute fn( plg.attribute() );
	AStringAttribute * r = new AStringAttribute;
	r->setLongName(fn.name().asChar() );
	r->setShortName(fn.shortName().asChar() );
	MString val;
    plg.getValue(val);
	r->setValue(val.asChar());
	return r;
}

ANumericAttribute * AttributeHelper::AsNumericData(const MPlug & plg)
{
	short va;
	int vb;
	float vc;
	double vd;
	bool ve;
	ANumericAttribute * r = NULL;
	MStatus stat;
	MFnNumericAttribute fn( plg.attribute(), &stat );
	if(!stat) {
		MGlobal::displayInfo(MString(" not a numberic attribute ") + plg.name());
		return r;
	}
	switch(fn.unitType()) {
       case MFnNumericData::kByte : 
			va = GetPlugValue<short>(plg);
			r = new AByteNumericAttribute(va);
            break;
       case MFnNumericData::kShort :
            va = GetPlugValue<short>(plg);
			r = new AShortNumericAttribute(va);
           break;
       case MFnNumericData::kInt:
			vb = GetPlugValue<int>(plg);
			r = new AIntNumericAttribute(vb);
           break;
       case MFnNumericData::kFloat:
			vc = GetPlugValue<float>(plg);
			r = new AFloatNumericAttribute(vc);
           break;
       case MFnNumericData::kDouble:
            vd = GetPlugValue<double>(plg);
			r = new ADoubleNumericAttribute(vd);
           break;
       case MFnNumericData::kBoolean:
            ve = GetPlugValue<bool>(plg);
			r = new ABooleanNumericAttribute(ve);
           break;
       default:
           MGlobal::displayInfo(fn.name() + " unknown numberic attribute type " + fn.unitType());
           break;
   }
   if(!r) return r;
   r->setLongName(fn.name().asChar() );
	r->setShortName(fn.shortName().asChar() );
   return r;
}

ACompoundAttribute * AttributeHelper::AsCompoundData(const MPlug & plg)
{
	ACompoundAttribute * r = new ACompoundAttribute;
	MFnCompoundAttribute fn(plg.attribute());
	r->setLongName(fn.name().asChar() );
	r->setShortName(fn.shortName().asChar() );
	
	const unsigned n = fn.numChildren();
    unsigned i;
    for(i=0; i < n; i++) {
        MObject child = fn.child(i);
		MPlug cp(plg.node(), child);
        ANumericAttribute * dc = AsNumericData(cp);
		if(dc) r->addChild(dc);
    }
	return r;
}

AEnumAttribute * AttributeHelper::AsEnumData(const MPlug & plg)
{
	AEnumAttribute * r = new AEnumAttribute;
	MFnEnumAttribute fn(plg.attribute());
	r->setLongName(fn.name().asChar() );
	r->setShortName(fn.shortName().asChar() );
	short minValue, maxValue;
    fn.getMin(minValue);
 	fn.getMax(maxValue);
	short val = GetPlugValue<short>(plg);
	r->setRange(minValue, maxValue);
	r->setValue(val);
	short i;
    for(i = minValue; i <= maxValue; i++) {
        MString fld = fn.fieldName(i);
        r->addField(i, fld.asChar());
    }
	return r;
}

bool AttributeHelper::HasNamedAttribute(MObject & attrib, 
                        const MObject & node, const std::string & name)
{
	MFnDependencyNode fn(node);
	MStatus stat;
	attrib = fn.attribute(MString(name.c_str()), &stat);
	return stat == MS::kSuccess;
}

bool AttributeHelper::IsStringAttr(MObject & entity)
{
	if(!entity.hasFn(MFn::kTypedAttribute)) return false;
	MFnTypedAttribute fn(entity);
	if(fn.attrType() != MFnData::kString) return false;
	return true;
}

bool AttributeHelper::IsEnumAttr(MObject & entity)
{ return entity.hasFn(MFn::kEnumAttribute); }

bool AttributeHelper::IsNumericAttr(MObject & entity, MFnNumericData::Type t)
{
	if(!entity.hasFn(MFn::kNumericAttribute)) return false;
	MFnNumericAttribute fn(entity);
	if(fn.unitType() != t) return false;
	return true;
}

bool AttributeHelper::AddStringAttr(MObject & attr, 
                            const MObject & node, 
									const std::string & nameLong, 
									const std::string & nameShort)
{
	MFnTypedAttribute fAttr;
	attr = fAttr.create(MString(nameLong.c_str()), MString(nameShort.c_str()), MFnData::kString);
	fAttr.setStorable(true);
	MFnDependencyNode fn(node);
	MStatus stat = fn.addAttribute( attr, MFnDependencyNode::kLocalDynamicAttr );
	return stat == MS::kSuccess;
}

bool AttributeHelper::AddNumericAttr(MObject & attr, 
                            const MObject & node, 
									const std::string & nameLong, 
									const std::string & nameShort,
									MFnNumericData::Type t)
{
	MFnNumericAttribute fAttr;
	attr = fAttr.create(MString(nameLong.c_str()), MString(nameShort.c_str()), t);
	fAttr.setStorable(true);
	fAttr.setKeyable(true);
	MFnDependencyNode fn(node);
	MStatus stat = fn.addAttribute( attr, MFnDependencyNode::kLocalDynamicAttr );
	return stat == MS::kSuccess;
}

bool AttributeHelper::AddEnumAttr(MObject & attr, 
                            const MObject & node, 
									const std::string & nameLong, 
									const std::string & nameShort,
									const std::map<short, std::string > & fields)
{
	MFnEnumAttribute fAttr;
    attr = fAttr.create(MString(nameLong.c_str()), MString(nameShort.c_str()));
    
	std::map<short, std::string >::const_iterator it = fields.begin();
	for(; it!=fields.end(); ++it) fAttr.addField(it->second.c_str(), it->first);
        
	fAttr.setStorable(true);
	fAttr.setKeyable(true);
    MFnDependencyNode fn(node);;
	MStatus stat = fn.addAttribute( attr, MFnDependencyNode::kLocalDynamicAttr );
	return stat == MS::kSuccess;
}

bool AttributeHelper::IsDirectAnimated(const MPlugArray & conns)
{
	if(conns.length() < 1) return false;

/// connected to a time-based animation curve node	
	MObject node = conns[0].node();
	return (node.hasFn( MFn::kAnimCurveTimeToAngular )  ||
			node.hasFn( MFn::kAnimCurveTimeToDistance )  ||
			node.hasFn( MFn::kAnimCurveTimeToUnitless ) );
}

bool AttributeHelper::IsDirectAnimated(const MPlug & attrib)
{
	MPlugArray conns;
	attrib.connectedTo ( conns, true, false);
	return IsDirectAnimated(conns);
}

MStatus AttributeHelper::CreateVector3FAttrib(MObject & a,
						MObject & ax, MObject & ay, MObject & az,
						const MString & longName, const MString & shortName,
						const float & dx, const float & dy, const float & dz)
{
	MFnNumericAttribute numFn;
	
	ax = numFn.create( longName+"x", shortName+"x", MFnNumericData::kFloat);
	numFn.setStorable(true);
	numFn.setKeyable(true);

	ay = numFn.create( longName+"y", shortName+"y", MFnNumericData::kFloat);
	numFn.setStorable(true);
	numFn.setKeyable(true);

	az = numFn.create( longName+"z", shortName+"z", MFnNumericData::kFloat);
	numFn.setStorable(true);
	numFn.setKeyable(true);

	a = numFn.create( longName, shortName, ax, ay, az );
	numFn.setStorable(true);
	numFn.setKeyable(true);
	numFn.setDefault(dx, dy, dz);
	
	return MS::kSuccess;
}

}
//:~
