/*
 *  RenderOptions.cpp
 *  aphid
 *
 *  Created by jian zhang on 1/13/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include "RenderOptions.h"
#include "BaseCamera.h"
RenderOptions::RenderOptions() 
{
	m_camera = 0;
	m_resX = 640;
	m_resY = 480;
	m_AASample = 4;
	m_maxSubdiv = 3;
	m_useDisplaySize = false;
}

RenderOptions::~RenderOptions() {}

int RenderOptions::AASample() const
{
	return m_AASample;
}

int RenderOptions::renderImageWidth() const
{
	return m_resX;
}

int RenderOptions::renderImageHeight() const
{
	return m_resY;
}

int RenderOptions::maxSubdiv() const
{
	return m_maxSubdiv;
}

bool RenderOptions::useDisplaySize() const
{
	return m_useDisplaySize;
}

void RenderOptions::setAASample(int x)
{
	m_AASample = x;
}

void RenderOptions::setRenderImageWidth(int x)
{
	m_resX = x;
}

void RenderOptions::setRenderImageHeight(int y)
{
	m_resY = y;
}

void RenderOptions::setMaxSubdiv(int x)
{
	m_maxSubdiv = x;
}

void RenderOptions::setUseDisplaySize(bool x)
{
	m_useDisplaySize = x;
}

void RenderOptions::setRenderCamera(BaseCamera * camera)
{
	m_camera = camera;
}

BaseCamera * RenderOptions::renderCamera() const
{
	return m_camera;
}
