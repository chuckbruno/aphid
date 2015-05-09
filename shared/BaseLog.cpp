/*
 *  BaseLog.cpp
 *  testnarrowpahse
 *
 *  Created by jian zhang on 3/29/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#include "BaseLog.h"
#include "boost/date_time/posix_time/posix_time.hpp"

using namespace boost::posix_time;

std::map<std::string, bool> BaseLog::VisitedPtr;

BaseLog::BaseLog(const std::string & fileName)
{
	m_file.open(fileName.c_str(), std::ios::trunc);
	m_file<<"start logging ";
	writeTime();
}

BaseLog::~BaseLog() 
{
	if(m_file.is_open())
		m_file.close();
}

void BaseLog::write(unsigned & i)
{ _write<unsigned>(i); }

void BaseLog::write(const std::string & os )
{ m_file<<os; }

void BaseLog::writeTime()
{ 
	const ptime now = second_clock::local_time();
	m_file<<" at "<<to_iso_extended_string(now)<<"\n";
}

void BaseLog::newLine()
{ m_file<<"\n"; }

void BaseLog::writeArraySize(const unsigned & n)
{ write(boost::str(boost::format(" [%1%] \n") % n)); }

void BaseLog::writeArrayIndex(const unsigned & n)
{ write(boost::str(boost::format(" %1%: ") % n)); }

void BaseLog::writeStruct1(char * data, const std::vector<std::pair<int, int> > & desc)
{
    std::vector<std::pair<int, int> >::const_iterator it = desc.begin();
    for(;it!=desc.end();++it)
        writeByTypeAndLoc(it->first, it->second, data);
    newLine();
}

void BaseLog::writeByTypeAndLoc(int type, int loc, char * data)
{
    switch(type) {
    case 0:
        _write<int>(*(int *)&data[loc]);
        break;
    case 1:
        _write<float>(*(float *)&data[loc]);
        break;
    default:
        ;
    }
}

void BaseLog::braceBegin(const std::string & notation, Frequency freq)
{ 
	if(!checkFrequency(freq, notation, 1)) return;
	
	m_file<<notation<<" {\n";
	m_pathToName.push_back(notation);
	//std::cout<<" last path { "<<m_pathToName.back();
}

void BaseLog::braceEnd(const std::string & notation, Frequency freq)
{ 
	if(m_pathToName.size() < 1) return;
	
	//std::cout<<" last path } "<<m_pathToName.back();
	
	if(notation!=m_pathToName.back()) return;
	
	m_file<<" } // end of "<<m_pathToName.back()<<"\n"; 
	m_pathToName.pop_back(); 
}

bool BaseLog::checkFrequency(Frequency freq, const std::string & notation,
							char ignorePath)
{
	if(freq == FIgnore) return true;
	std::string fpn = notation;
	if(!ignorePath) fpn = fullPathName(notation);
	if(freq == FOnce) {
        if(VisitedPtr.find(fpn) != VisitedPtr.end()) {
			return false;
		}
		else
			VisitedPtr[fpn] = true;
    }
	return true;
}

std::string BaseLog::fullPathName(const std::string & name)
{
	if(m_pathToName.size() < 1) return name;
	
	std::stringstream sst;
	sst.str("/");
	std::vector<std::string >::const_iterator it = m_pathToName.begin();
	for(; it != m_pathToName.end(); ++it) {
		sst<<*it<<"/";
	}
	sst<<name;
	return sst.str();
}
