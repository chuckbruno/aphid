/*
 *  HesperisFile.cpp
 *  hesperis
 *
 *  Created by jian zhang on 4/21/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#include "HesperisFile.h"
#include <AllHdf.h>
#include <HWorld.h>
#include <HCurveGroup.h>
#include <BaseBuffer.h>
#include <HFrameRange.h>
#include <HTransform.h>
#include <HTetrahedronMesh.h>
#include <HTriangleMesh.h>
#include <ATriangleMesh.h>
#include <ATetrahedronMesh.h>
#include <BaseTransform.h>
#include <GeometryArray.h>
#include <SHelper.h>
#include <sstream>
#include <boost/format.hpp>

AFrameRange HesperisFile::Frames;
bool HesperisFile::DoStripNamespace = true;

HesperisFile::HesperisFile() {}
HesperisFile::HesperisFile(const char * name) : HFile(name) 
{
	m_readComp = RNone;
	m_writeComp = WCurve;
}

HesperisFile::~HesperisFile() {}

void HesperisFile::setReadComponent(ReadComponent comp)
{ m_readComp = comp; }

void HesperisFile::setWriteComponent(WriteComponent comp)
{ m_writeComp = comp; }

bool HesperisFile::doWrite(const std::string & fileName)
{
	if(!HObject::FileIO.open(fileName.c_str(), HDocument::oReadAndWrite)) {
		setLatestError(BaseFile::FileNotWritable);
		return false;
	}
	
	HWorld grpWorld;
	grpWorld.save();
	
	switch (m_writeComp) {
		case WCurve:
			writeCurve();
			break;
		case WTetra:
			writeTetrahedron();
			break;
		case WTri:
			writeTriangle();
			break;
		case WTransform:
			writeTransform();
			break;
		default:
			break;
	}
	
	writeFrames();
	grpWorld.close();
	HObject::FileIO.close();
	
	std::cout<<" finished writing hesperis file at "<<grpWorld.modifiedTimeStr()<<"\n";
	
	return true;
}

bool HesperisFile::writeTransform()
{
	std::map<std::string, BaseTransform *>::iterator ittrans = m_transforms.begin();
	for(; ittrans != m_transforms.end(); ++ittrans) {
		std::cout<<" write transform "<<worldPath(ittrans->first)
        <<"\n";
		HTransform grp(worldPath(ittrans->first));
		grp.save(ittrans->second);
		grp.close();
	}
	return true;
}

bool HesperisFile::writeCurve()
{
	std::map<std::string, CurveGroup *>::iterator itcurve = m_curves.begin();
	for(; itcurve != m_curves.end(); ++itcurve) {
		std::cout<<" write curve "<<worldPath(itcurve->first)
        <<"\n";
		HCurveGroup grpCurve(worldPath(itcurve->first));
		grpCurve.save(itcurve->second);
		grpCurve.close();
	}
	return true;
}

bool HesperisFile::writeTetrahedron()
{
	std::map<std::string, ATetrahedronMesh *>::iterator it = m_terahedrons.begin();
	for(; it != m_terahedrons.end(); ++it) {
		std::cout<<" write tetrahedron mesh "<<worldPath(it->first)
		<<"\n";
		HTetrahedronMesh grp(worldPath(it->first));
		grp.save(it->second);
		grp.close();
	}
	return true;
}

bool HesperisFile::writeTriangle()
{
	std::map<std::string, ATriangleMesh *>::iterator it = m_triangleMeshes.begin();
	for(; it != m_triangleMeshes.end(); ++it) {
		std::cout<<" write triangle mesh "<<worldPath(it->first)
		<<"\n";
		HTriangleMesh grp(worldPath(it->first));
		grp.save(it->second);
		grp.close();
	}
	return true;
}

void HesperisFile::addTransform(const std::string & name, BaseTransform * data)
{ m_transforms[checkPath(name)] = data; }

void HesperisFile::addCurve(const std::string & name, CurveGroup * data)
{ m_curves[checkPath(name)] = data; }

void HesperisFile::addTetrahedron(const std::string & name, ATetrahedronMesh * data)
{ m_terahedrons[checkPath(name)] = data; }

void HesperisFile::addTriangleMesh(const std::string & name, ATriangleMesh * data)
{ m_triangleMeshes[checkPath(name)] = data; }

bool HesperisFile::doRead(const std::string & fileName)
{
	if(!HFile::doRead(fileName)) return false;
	
	std::cout<<" reading curves file "<<fileName<<"\n";
	HWorld grpWorld;
	grpWorld.load();
	
	switch (m_readComp) {
		case RCurve:
			readCurve();
			break;
		case RTetra:
            listTetrahedron(&grpWorld);
			readTetrahedron();
			break;
		case RTri:
			listTriangle(&grpWorld);
			readTriangle();
		default:
			break;
	}
	
	readFrames(&grpWorld);
	grpWorld.close();
	
	std::cout<<" finished reading hesperis file modified at "<<grpWorld.modifiedTimeStr()<<"\n";
	
	return true;
}

bool HesperisFile::readCurve()
{
	bool allValid = true;
	std::map<std::string, CurveGroup *>::iterator itcurve = m_curves.begin();
	for(; itcurve != m_curves.end(); ++itcurve) {
		std::cout<<" read curve "<<worldPath(itcurve->first)
		<<"\n";
		HCurveGroup grpCurve(worldPath(itcurve->first));
		if(!grpCurve.load(itcurve->second)) {
			std::cout<<" cannot load "<<worldPath(itcurve->first)
			<<"\n";
			allValid = false;
		}
		
		grpCurve.close();
	}
	
	if(!allValid)
		std::cout<<" encounter problem(s) reading curves.\n";

	return allValid;
}

bool HesperisFile::listTetrahedron(HBase * grp)
{
    std::vector<std::string > tetraNames;
	grp->lsTypedChild<HTetrahedronMesh>(tetraNames);
	
	std::vector<std::string>::const_iterator it = tetraNames.begin();
	for(;it!=tetraNames.end();++it) {
		addTetrahedron(*it, new ATetrahedronMesh);
	}
	return true;
}

bool HesperisFile::readTetrahedron()
{
    bool allValid = true;
	std::map<std::string, ATetrahedronMesh *>::iterator it = m_terahedrons.begin();
	for(; it != m_terahedrons.end(); ++it) {
		std::cout<<" read tetrahedron mesh "<<it->first<<"\n";
		HTetrahedronMesh grp(it->first);
		if(!grp.load(it->second)) {
			std::cout<<" cannot load "<<it->first;
			allValid = false;
		}
		grp.close();
	}
	
	if(!allValid)
		std::cout<<" encounter problem(s) reading tetrahedrons.\n";

	return allValid;
}

bool HesperisFile::listTriangle(HBase * grp)
{
	std::vector<std::string > triNames;
	grp->lsTypedChild<HTriangleMesh>(triNames);
	
	std::vector<std::string>::const_iterator it = triNames.begin();
	for(;it!=triNames.end();++it) {
		addTriangleMesh(*it, new ATriangleMesh);
	}
	return true;
}

bool HesperisFile::readTriangle()
{
	bool allValid = true;
	std::map<std::string, ATriangleMesh *>::iterator it = m_triangleMeshes.begin();
	for(; it != m_triangleMeshes.end(); ++it) {
		std::cout<<" read triangle mesh "<<it->first<<"\n";
		HTriangleMesh grp(it->first);
		if(!grp.load(it->second)) {
			std::cout<<" cannot load "<<it->first;
			allValid = false;
		}
		grp.close();
	}
	
	if(!allValid)
		std::cout<<" encounter problem(s) reading triangles.\n";

	return allValid;
}

void HesperisFile::extractTetrahedronMeshes(GeometryArray * dst)
{
    dst->create(m_terahedrons.size());
	unsigned i = 0;
	std::map<std::string, ATetrahedronMesh *>::const_iterator it = m_terahedrons.begin();
	for(; it != m_terahedrons.end(); ++it) {
		dst->setGeometry(it->second, i);
		i++;
	}
	dst->setNumGeometries(i);
}

void HesperisFile::extractTriangleMeshes(GeometryArray * dst)
{
	dst->create(m_triangleMeshes.size());
	unsigned i = 0;
	std::map<std::string, ATriangleMesh *>::const_iterator it = m_triangleMeshes.begin();
	for(; it != m_triangleMeshes.end(); ++it) {
		dst->setGeometry(it->second, i);
		i++;
	}
	dst->setNumGeometries(i);
}

std::string HesperisFile::worldPath(const std::string & name) const
{ 
	if(SHelper::IsPullPath(name))
		return boost::str(boost::format("/world%1%") % name); 
	return boost::str(boost::format("/world/%1%") % name); 
}

std::string HesperisFile::checkPath(const std::string & name) const
{
	std::string r(name);
	if(DoStripNamespace) SHelper::removeAnyNamespace(r);
	
	return HObject::ValidPathName(r);
}

bool HesperisFile::writeFrames()
{
	if(!Frames.isValid()) return false;
	HFrameRange g("/world/.frames");
	g.save(&Frames);
	g.close();
	return true;
}

bool HesperisFile::readFrames(HBase * grp)
{
	Frames.reset();
	if(!grp->hasNamedChild(".frames")) {
		return false;
	}
	HFrameRange g("/world/.frames");
	g.load(&Frames);
	g.close();
	return true;
}
//:~