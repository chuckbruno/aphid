/*
 *  HWorld.cpp
 *  mallard
 *
 *  Created by jian zhang on 9/27/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "HWorld.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/local_time_adjustor.hpp"
#include "boost/date_time/c_local_time_adjustor.hpp"
using namespace boost::posix_time;

namespace aphid {

HWorld::HWorld() : HBase("/world"), m_modifiedTime(0) 
{
}

char HWorld::save()
{
	const ptime now = second_clock::local_time();
	ptime ref(from_iso_string("20120131T235959"));
	
	time_duration td = now - ref;
	m_modifiedTime = td.total_seconds();
	
	if(!hasNamedAttr(".time"))
		addIntAttr(".time");
	writeIntAttr(".time", &m_modifiedTime);
	
	return 1;
}

char HWorld::load()
{
	if(hasNamedAttr(".time"))
		readIntAttr(".time", &m_modifiedTime);

	return 1;
}

std::string HWorld::modifiedTimeStr() const
{
	time_duration td = seconds(m_modifiedTime);
	ptime rec = ptime(from_iso_string("20120131T235959")) + td;
	return to_iso_extended_string(rec);
}

}