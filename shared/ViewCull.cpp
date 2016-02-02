/*
 *  ViewCull.cpp
 *  proxyPaint
 *
 *  Created by jian zhang on 2/2/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#include "ViewCull.h"
#include <GjkIntersection.h>

ViewCull::ViewCull() : m_enabled(false) {}
ViewCull::~ViewCull() {}
	
void ViewCull::enable()
{ m_enabled = true; }

void ViewCull::disable()
{ m_enabled = false; }

void ViewCull::setFrustum(const float & horizontalApeture,
			const float & verticalApeture,
			const float & focalLength,
			const float & clipNear,
			const float & clipFar)
{
	m_hfov = horizontalApeture * 0.5f / ( focalLength * 0.03937f );
	m_aspectRatio = verticalApeture / horizontalApeture;
	m_frustum.set(m_hfov, 
				m_aspectRatio,
				clipNear,
				clipFar,
				m_space);
}
	
Matrix44F *	ViewCull::cameraSpaceP()
{ return &m_space; }

Matrix44F * ViewCull::cameraInvSpaceP()
{ return &m_invSpace; }

const Matrix44F & ViewCull::cameraSpace() const
{ return m_space; }

const Matrix44F & ViewCull::cameraInvSpace() const
{ return m_invSpace; }

const AFrustum & ViewCull::frustum() const
{ return m_frustum; }

bool ViewCull::cullByFrustum(const Vector3F & center, const float & radius) const
{
	gjk::Sphere B(center, radius );
	if( gjk::Intersect1<AFrustum, gjk::Sphere>::Evaluate(m_frustum, B) )
		return false;
	return true;
}

void ViewCull::ndc(const Vector3F & cameraP, float & coordx, float & coordy) const
{
	float d = -cameraP.z;
	if(d<1.f) d= 1.f;
	float h_max = d * m_hfov;
	float h_min = -h_max;
	float v_max = h_max * m_aspectRatio;
	float v_min = -v_max;
	coordx = (cameraP.x - h_min) / (h_max - h_min);
	coordy = (cameraP.y - v_min) / (v_max - v_min);
	if(coordx < 0.f) coordx = 0.f;
	if(coordx > .997f) coordx = .997f;
	if(coordy < 0.f) coordy = 0.f;
	if(coordy > .997f) coordy = .997f;
}
//:~