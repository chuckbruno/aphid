/*
 *  CurveGroup.cpp
 *  hesperis
 *
 *  Created by jian zhang on 4/21/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#include <geom/CurveGroup.h>

namespace aphid {

CurveGroup::CurveGroup() 
{
	m_numCurves = m_numPoints = 0;
	m_points = 0;
	m_counts = 0;
	m_degrees = 0;
}

CurveGroup::~CurveGroup() 
{
	clear();
}

void CurveGroup::create(unsigned n, unsigned ncvs)
{
	clear();
	m_numCurves = n;
	m_numPoints = ncvs;
	
	m_counts = new unsigned[m_numCurves];
	m_points = new Vector3F[m_numPoints];
	m_degrees = new int[m_numCurves];
}

void CurveGroup::clear()
{
	m_numCurves = 0;
	m_numPoints = 0;
	if(m_degrees) delete[] m_degrees;
	if(m_points) delete[] m_points;
	if(m_counts) delete[] m_counts;
}

Vector3F * CurveGroup::points()
{ return m_points; }

unsigned * CurveGroup::counts()
{ return m_counts; }

int * CurveGroup::curveDegrees()
{ return m_degrees; }

const unsigned & CurveGroup::numPoints() const
{ return m_numPoints; }

const unsigned & CurveGroup::numCurves() const
{ return m_numCurves; }

void CurveGroup::setAllCurveDegree(int x)
{
	for(int i=0; i<m_numCurves; ++i ) {
		m_degrees[i] = x;
	}
}

void CurveGroup::verbose() const
{
	std::cout<<" curve group n curves: "<<m_numCurves;
	std::cout<<" n cvs: "<<m_numPoints<<"\n";
}

}
//:~