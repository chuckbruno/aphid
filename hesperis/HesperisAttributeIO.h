/*
 *  HesperisAttributeIO.h
 *  opium
 *
 *  Created by jian zhang on 10/18/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include "HesperisAnimIO.h"

namespace aphid {
    
class HesperisAttributeIO : public HesperisAnimIO {
public:
	HesperisAttributeIO();
	virtual ~HesperisAttributeIO();
	
	static bool WriteAttributeBoundle(const std::map<std::string, char> & attrNames,
                            const MPlugArray & attribs,
                            const std::string & parentName,
                            HesperisFile * file);
	static bool WriteAttributes(const MPlugArray & attribs, HesperisFile * file);
    static bool AddAttribute(const MPlug & attrib, HesperisFile * file);
	
	static bool ReadAttributeBundle(const MObject &target = MObject::kNullObj);
	static bool ReadAttributeBundle(HBase * parent, const MObject &target);
	static bool ReadAttributes(const MObject &target = MObject::kNullObj);
	static bool ReadAttributes(HBase * parent, const MObject &target);
	
	static bool BeginBakeAttribute(const std::string & attrName, ANumericAttribute *data);
	static bool EndBakeAttribute(const std::string & attrName, ANumericAttribute *data);
	static bool BakeAttribute(const std::string & attrName, ANumericAttribute *data);
	static bool BeginBakeEnum(const std::string & attrName, AEnumAttribute *data);
	static bool EndBakeEnum(const std::string & attrName, AEnumAttribute *data);
	static bool BakeEnum(const std::string & attrName, AEnumAttribute *data);
	static void ClearBakeData();
	
protected:
	static std::map<std::string, HObject * > MappedBakeData;
	
private:
    static bool ReadAttributeBundle(const ABundleAttribute * d,
                        const MObject &target);
	static bool ReadAttribute(MObject & dst, 
	                    AAttribute * data, 
	                    const MObject &target);
	static bool ReadStringAttribute(MObject & dst, AStringAttribute * data, 
	                    const MObject &target);
	static bool ReadNumericAttribute(MObject & dst, ANumericAttribute * data, 
	                    const MObject &target);
	static bool ReadCompoundAttribute(MObject & dst, ACompoundAttribute * data, 
	                    const MObject &target);
	static bool ReadEnumAttribute(MObject & dst, AEnumAttribute * data, 
	                    const MObject &target);
	static HObject * CreateBake(HBase * grp, ANumericAttribute::NumericAttributeType typ,
							const std::string & attrName, const std::string & dataName,
							bool &stat);
	static bool InsertDataValue(HObject * grp, ANumericAttribute *data);
	static bool FinishInsertDataValue(HObject * grp, ANumericAttribute::NumericAttributeType typ);
	
	template<typename Td, typename Tv>
	static bool InsertValue(HObject * g, Tv v)
	{
		static_cast<Td *>(g)->insert((char *)&v);
		return true;
	}
	
	template<typename Td>
	static bool FinishInsertValue(HObject * g)
	{
		static_cast<Td *>(g)->finishInsert();
		// static_cast<Td *>(g)->printValues();
		return true;
	}
	
	static bool ConnectBaked(HBase * parent, AAttribute * data, 
	                        const MObject & entity, MObject & attr);
	
};

}
