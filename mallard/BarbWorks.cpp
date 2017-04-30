/*
 *  BarbWorks.cpp
 *  mallard
 *
 *  Created by jian zhang on 1/1/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include "BarbWorks.h"
#include "MlSkin.h"
#include "MlFeather.h"
#include "MlCalamus.h"
#include <AdaptableStripeBuffer.h>
#include <BlockStripeBuffer.h>
#include <RenderEngine.h>
#include <boost/thread.hpp>
#include <boost/timer.hpp>
#include <EasemodelUtil.h>
BarbWorks::BarbWorks() 
{
	m_skin = 0;
	m_stripes = new BlockStripeBuffer;
}

BarbWorks::~BarbWorks() {}

void BarbWorks::setSkin(MlSkin * skin)
{
	m_skin = skin;
}

MlSkin * BarbWorks::skin() const
{
	return m_skin;
}

unsigned BarbWorks::numFeathers() const
{
	return m_skin->numFeathers();
}

void BarbWorks::computeFeather(MlCalamus * c)
{
	Vector3F p;
	skin()->getPointOnBody(c, p);
	
	Matrix33F space;
	skin()->calamusSpace(c, space);
	computeFeather(c, p, space);
}

void BarbWorks::computeFeather(MlCalamus * c, const Vector3F & p, const Matrix33F & space)
{
	skin()->touchBy(c, p, space);
	c->bendFeather(p, space);
	c->curlFeather();
	c->computeFeatherWorldP(p, space);
}

char BarbWorks::isTimeCached(const std::string & stime)
{
	return isCached("/ang", stime) > 0;
}

void BarbWorks::openCache(const std::string & stime)
{
	useDocument();
	openEntry("/ang");
	openSliceFloat("/ang", stime);
	openEntry("/p");
	openSliceVector3("/p", stime);
	openEntry("/tang");
	openSliceMatrix33("/tang", stime);
}

void BarbWorks::closeCache(const std::string & stime)
{
	closeSlice("/ang", stime);
	closeEntry("/ang");
	closeSlice("/p", stime);
	closeEntry("/p");
	closeSlice("/tang", stime);
	closeEntry("/tang");
}

void BarbWorks::createBarbBuffer(RenderEngine * engine)
{
	if(!skin()) return;
	m_percentFinished = 0.f;
	boost::timer met;
	met.restart();
	const unsigned nc = numFeathers();
	if(nc < 1) return;
	m_stripes->initialize();
	unsigned i;
	
	BoundingBox box;
	Matrix33F space;
	Vector3F p;
	unsigned nline = 0;
	unsigned nv = 0;
	unsigned sd = 1984;
	float rd, lod;
	float minLod = 100;
	float maxLod = -100;
	
	std::clog<<"build feather count: "<<nc<<"\n";
	for(i = 0; i < nc; i++) {
		MlCalamus * c = skin()->getCalamus(i);
		skin()->calamusSpace(c, space);
		skin()->getPointOnBody(c, p);
		
		computeFeather(c, p, space);

		MlFeather * f = c->feather();
		f->getBoundingBox(box);
		
		f->setSeed(sd++);
		f->separateVane();
		
		rd = f->scaledShaftLength();
		
		lod = computeLOD(p, rd, f->numSegment() * 8);
		
		if(lod > maxLod) maxLod = lod;
		if(lod < minLod) minLod = lod;
		
		f->sampleColor(lod);
		f->samplePosition(lod);
		
		AdaptableStripeBuffer * src = f->stripe();

		m_stripes->append(src);
		
		nline += f->numStripe();
		nv += f->numStripePoints();
		
		m_percentFinished = (float)i/(float)nc;
		boost::this_thread::interruption_point();
	}
	std::cout<<" barb processed in "<<met.elapsed()<<" seconds\n";
	std::cout<<"n curve "<<nline<<" n p "<<nv<<"\n";
	std::cout<<"lod range ("<<minLod<<" , "<<maxLod<<")\n";
	std::cout<<"n blocks "<<numBlocks()<<"\n";
	m_percentFinished = 1.f;
	std::cout<<"start render";
	engine->render();
}

void BarbWorks::clearBarbBuffer()
{
	m_stripes->clear();
}

float BarbWorks::percentFinished() const
{
	return m_percentFinished;
}

unsigned BarbWorks::numBlocks() const
{
    return m_stripes->numBlocks();
}
	
AdaptableStripeBuffer * BarbWorks::block(unsigned idx) const
{
    return m_stripes->block(idx);
}

void BarbWorks::clearBlock(unsigned idx)
{
    m_stripes->clearBlock(idx);
}

bool BarbWorks::dumpModel(const std::string & name)
{
    if(!skin()) return false;
	// m_percentFinished = 0.f;
	boost::timer met;
	met.restart();
	const unsigned nc = numFeathers();
	if(nc < 1) return false;
	m_stripes->initialize();
	unsigned i;
	
	BoundingBox box;
	Matrix33F space;
	//
	//unsigned nline = 0;
	unsigned np = 0;
	unsigned nfv = 0;
	unsigned nf = 0;
	//unsigned sd = 1984;
	//float rd, lod;
	//float minLod = 100;
	//float maxLod = -100;
	
	std::clog<<"build feather count: "<<nc<<"\n";
	for(i = 0; i < nc; i++) {
		MlCalamus * c = skin()->getCalamus(i);
		//skin()->calamusSpace(c, space);
		//skin()->getPointOnBody(c, p);
		
		//computeFeather(c, p, space);

		MlFeather * f = c->feather();
		np += f->numOnEdgeP();
		nfv += f->numOnEdgeFV();
		nf += f->numOnEdgeF();
		/*f->getBoundingBox(box);
		
		f->setSeed(sd++);
		f->separateVane();
		
		rd = f->scaledShaftLength();
		
		lod = computeLOD(p, rd, f->numSegment() * 8);
		
		if(lod > maxLod) maxLod = lod;
		if(lod < minLod) minLod = lod;
		
		f->sampleColor(lod);
		f->samplePosition(lod);
		
		AdaptableStripeBuffer * src = f->stripe();

		m_stripes->append(src);
		
		nline += f->numStripe();
		nv += f->numStripePoints();*/
		
		// m_percentFinished = (float)i/(float)nc;
	}
	std::cout<<" n p "<<np<<"\n";
	std::cout<<" n face "<<nf<<"\n";
	std::cout<<" n face vertex "<<nfv<<"\n";
	
	unsigned * fv = new unsigned[nfv];
	Vector3F * p = new Vector3F[np];
	float * u = new float[np];
	float * v = new float[np];
	unsigned * fc = new unsigned[nf];
	for(i = 0; i < nf; i++) fc[i] = 4;
	
	unsigned cp = 0;
	unsigned cfv = 0;
	unsigned offset = 0;
	Vector3F q;
	for(i = 0; i < nc; i++) {
		MlCalamus * c = skin()->getCalamus(i);
		skin()->calamusSpace(c, space);
		skin()->getPointOnBody(c, q);
		
		computeFeather(c, q, space);

		MlFeather * f = c->feather();
		
		f->dumpWP(p, u, v, cp);
		f->dumpFV(fv, cfv, offset);
	}
	
	ESMUtil::Export(name.c_str(), np, nf, nfv,
	                p, fc, fv,
	                u, v, fv);
	
	delete[] fv;
	delete[] p;
	delete[] u;
	delete[] v;
	delete[] fc;
	std::cout<<" model processed in "<<met.elapsed()<<" seconds\n";
	
	//std::cout<<"n blocks "<<numBlocks()<<"\n";
	// m_percentFinished = 1.f;
    return true;
}
