/*
 *  FitBccMeshBuilder.cpp
 *  testbcc
 *
 *  Created by jian zhang on 4/27/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#include "FitBccMeshBuilder.h"
#include <BezierCurve.h>
#include "BccOctahedron.h"
#include "bcc_common.h"
#include <KdTreeDrawer.h>
#include "CurveSampler.h"
#include "SampleGroup.h"
//#define DBG_PRINT

unsigned FitBccMeshBuilder::MinimumUGrid = 3;

FitBccMeshBuilder::FitBccMeshBuilder() 
{
    m_octa = 0;
	m_sampler = new CurveSampler;
	m_reducer = new SampleGroup;
	m_startPoints = 0;
	m_tetraDrift = 0;
}

FitBccMeshBuilder::~FitBccMeshBuilder() 
{ 
	cleanup(); 
	delete m_sampler;
	delete m_reducer;
	if(m_startPoints) delete[] m_startPoints;
	if(m_tetraDrift) delete[] m_tetraDrift;
}

void FitBccMeshBuilder::cleanup()
{
    if(m_octa) delete[] m_octa;
}
    
void FitBccMeshBuilder::build(GeometryArray * geos, 
					unsigned & ntet, unsigned & nvert, unsigned & nstripes)
{
	tetrahedronP.clear();
	tetrahedronInd.clear();
	pointDrifts.clear();
	indexDrifts.clear();
	
    const unsigned n = geos->numGeometries();
	if(m_startPoints) delete[] m_startPoints;
	m_startPoints = new Vector3F[n];
	if(m_tetraDrift) delete[] m_tetraDrift;
	m_tetraDrift = new unsigned[n];
	
    unsigned i=0;
    for(;i<n;i++) {
        pointDrifts.push_back(tetrahedronP.size());
        indexDrifts.push_back(tetrahedronInd.size());
		m_tetraDrift[i] = tetrahedronInd.size() / 4;
        build((BezierCurve *)geos->geometry(i),
			   i);
	}
	
	ntet = tetrahedronInd.size()/4;
	nvert = tetrahedronP.size();
	nstripes = n;
}

void FitBccMeshBuilder::build(BezierCurve * curve,
								unsigned curveIdx)
{
	const unsigned lastNumTet = tetrahedronInd.size();
    cleanup();
	
	m_sampler->begin();
	m_sampler->process(curve, EstimatedGroupSize);
	m_sampler->end();
	
	const unsigned ns = curve->numSegments();
	
	float suml = 0.f;
	BezierSpline spl;
	
	unsigned i=0;
	for(;i<ns;i++) {
		curve->getSegmentSpline(i, spl);
		suml += BezierCurve::splineLength(spl);
	}

	unsigned numGroups = (int)(suml/EstimatedGroupSize);
    if(numGroups < MinimumUGrid) numGroups = MinimumUGrid;
    
#ifdef DBG_PRINT
	std::cout<<" total length "<<suml<<"\n"
	<<" estimate n groups "<<numGroups;
#endif
	
	const unsigned numSamples = m_sampler->numSamples();
	Vector3F * samples = m_sampler->samples();
	
	m_reducer->compute(samples, numSamples, numGroups);
	
	float * groupSize = m_reducer->groupSize();
    Vector3F * groupCenter = m_reducer->groupCentroid();
	
	m_startPoints[curveIdx] = groupCenter[0];

	int vv[2];
	int ee[2];
	float dV, dE;
	Vector3F octDir, a, b, c, d;
	
	m_octa = new BccOctahedron[numGroups];
	for(i=0; i<numGroups;i++) {
		if(i<1) {
			octDir = (groupCenter[1] - groupCenter[0]) * .5f;
		}
		else if(i==numGroups-1) {
			octDir = (groupCenter[numGroups-1] - groupCenter[numGroups-2]) * .5f;
		}
		else {
			octDir = (groupCenter[i] - groupCenter[i-1]) * .25f 
					+ (groupCenter[i+1] - groupCenter[i]) * .25f;

		}
		
		m_octa[i].create(groupCenter[i], octDir, groupSize[i]);
		
		if(i>0) {
			dV = m_octa[i].movePoleCost(vv, m_octa[i-1]);
			
			dE = m_octa[i].moveEdgeCost(ee, m_octa[i-1]);
			
			BccOctahedron::moveEdges(m_octa[i], ee[0], m_octa[i-1], ee[1], tetrahedronP);
			
			BccOctahedron::connectDifferentAxis(m_octa[i], 
												m_octa[i-1], tetrahedronP);
		}
		
		m_octa[i].createTetrahedron(tetrahedronP, tetrahedronInd);
		
		if(i>0) {
			BccOctahedron::add2GapTetrahedron(m_octa[i], ee[0], m_octa[i-1], ee[1],
													tetrahedronInd);
		}
	}
}

void FitBccMeshBuilder::drawOctahedron(KdTreeDrawer * drawer)
{
	for(unsigned i=0; i<m_reducer->numGroups(); i++)
		drawOctahedron(drawer, m_octa[i]);
}

void FitBccMeshBuilder::drawOctahedron(KdTreeDrawer * drawer, BccOctahedron & octa)
{
	Vector3F a, b;
	glColor3f(0.f, 0.8f, 0.f);

	drawer->arrow(octa.p()[0], octa.p()[1]);
	
	glColor3f(0.8f, 0.f, 0.f);
	
	int i;
	for(i=0;i<8;i++) {
		octa.getEdge(a, b, i);
		drawer->arrow(a, b);
	}
	
	glColor3f(0.f, 0.f, 0.8f);
	
	for(i=8;i<12;i++) {
		octa.getEdge(a, b, i);
		drawer->arrow(a, b);
	}
}

void FitBccMeshBuilder::drawSamples(KdTreeDrawer * drawer)
{
	const unsigned numSamples = m_sampler->numSamples();
	Vector3F * samples = m_sampler->samples();
	
	glDisable(GL_DEPTH_TEST);
	glColor3f(1.f, 0.f, 0.f);
	glBegin(GL_POINTS);
	unsigned i = 0;
	for(;i<numSamples;i++) glVertex3fv((float *)&samples[i]);
	glEnd();
	
	glColor3f(0.f, 1.f, 1.f);
	for(i=0;i<m_reducer->numGroups()-1;i++) drawer->arrow(m_reducer->groupCentroid()[i], m_reducer->groupCentroid()[i+1]);
	
}

Vector3F * FitBccMeshBuilder::startPoints()
{ return m_startPoints; }

unsigned * FitBccMeshBuilder::tetrahedronDrifts()
{ return m_tetraDrift; }

void FitBccMeshBuilder::addAnchors(ATetrahedronMesh * mesh, unsigned n, KdIntersection * anchorMesh)
{
	unsigned i, j, k;
	Geometry::ClosestToPointTestResult cls;
	for(i=0; i< n; i++) {
		cls.reset(startPoints()[i], 1e8f);
		anchorMesh->closestToPoint(&cls);
		unsigned anchorTri = cls._icomponent;
		addAnchor(mesh, i, startPoints()[i], anchorTri);
	}
}
//:~