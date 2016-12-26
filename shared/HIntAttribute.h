/*
 *  HIntAttribute.h
 *  helloHdf
 *
 *  Created by jian zhang on 12/21/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include <h5/HAttribute.h>

namespace aphid {

class HIntAttribute : public HAttribute
{
public:
	HIntAttribute(const std::string & path);
	~HIntAttribute() {}
	
	virtual char write(int *data);
	virtual char read(int *data);
};

}