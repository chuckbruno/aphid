/*
 *  HCurveGroup.h
 *  hesperis
 *
 *  Created by jian zhang on 4/21/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once
#include <h5/HBase.h>

namespace aphid {

class CurveGroup;
class HCurveGroup : public HBase {
public:
	HCurveGroup(const std::string & path);
	virtual ~HCurveGroup();
	
	char verifyType();
	virtual char save(CurveGroup * c);
	virtual char load(CurveGroup * c);
	
private:
	
};

}