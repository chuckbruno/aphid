/*
 *  shapeDrawer.cpp
 *  qtbullet
 *
 *  Created by jian zhang on 7/17/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#ifdef WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include "shapeDrawer.h"

void ShapeDrawer::setGrey(float g)
{
    glColor3f(g, g, g);
}

void ShapeDrawer::setColor(float r, float g, float b)
{
	glColor3f(r, g, b);
}

void ShapeDrawer::box(float width, float height, float depth)
{
	glBegin(GL_LINES);
	glColor3f(1.f, 0.f, 0.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(width, 0.f, 0.f);
	
	glColor3f(0.f, 1.f, 0.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, height, 0.f);
	
	glColor3f(0.f, 0.f, 1.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, 0.f, depth);
	
	glColor3f(0.23f, 0.23f, 0.24f);
	
	glVertex3f(width, 0.f, 0.f);
	glVertex3f(width, 0.f, depth);
	
	glVertex3f(width, 0.f, depth);
	glVertex3f(0.f, 0.f, depth);
	
	glVertex3f(0.f, height, 0.f);
	glVertex3f(width, height, 0.f);
	
	glVertex3f(width, height, 0.f);
	glVertex3f(width, height, depth);
	
	glVertex3f(width, height, depth);
	glVertex3f(0.f, height, depth);
	
	glVertex3f(0.f, height, depth);
	glVertex3f(0.f, height, 0.f);
	
	glVertex3f(width, 0.f, 0.f);
	glVertex3f(width, height, 0.f);
	
	glVertex3f(width, 0.f, depth);
	glVertex3f(width, height, depth);
	
	glVertex3f(0.f, 0.f, depth);
	glVertex3f(0.f, height, depth);
	glEnd();
}

void ShapeDrawer::solidCube(float x, float y, float z, float size)
{
	glBegin(GL_QUADS);
	
// bottom
	glVertex3f(x, y, z);
	glVertex3f(x + size, y, z);
	glVertex3f(x + size, y, z + size);
	glVertex3f(x, y, z + size);

// top
	glVertex3f(x, y+ size, z);
	glVertex3f(x + size, y+ size, z);
	glVertex3f(x + size, y+ size, z + size);
	glVertex3f(x, y+ size, z + size);
	
// back	
	glVertex3f(x, y, z);
	glVertex3f(x, y + size, z);
	glVertex3f(x + size, y + size, z);
	glVertex3f(x + size, y, z);
	
// front	
	glVertex3f(x, y, z + size);
	glVertex3f(x, y + size, z + size);
	glVertex3f(x + size, y + size, z + size);
	glVertex3f(x + size, y, z + size);

// left
	glVertex3f(x, y, z);
	glVertex3f(x, y, z + size);
	glVertex3f(x, y + size, z + size);
	glVertex3f(x, y + size, z);
	
// right
	glVertex3f(x + size, y, z);
	glVertex3f(x + size, y, z + size);
	glVertex3f(x + size, y + size, z + size);
	glVertex3f(x + size, y + size, z);
	glEnd();
}

void ShapeDrawer::end()
{
    if(m_wired) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnd();
}

void ShapeDrawer::beginSolidTriangle()
{
	glBegin(GL_TRIANGLES);
}

void ShapeDrawer::beginWireTriangle()
{
    m_wired = 1;
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_TRIANGLES);
}

void ShapeDrawer::beginLine()
{
	glBegin(GL_LINES);
}

void ShapeDrawer::beginPoint()
{
	glBegin(GL_POINTS);
}

void ShapeDrawer::aVertex(float x, float y, float z)
{
	glVertex3f(x, y, z);
}

void ShapeDrawer::drawVertex(const Polytode * poly)
{
	const int numV = poly->getNumVertex();
	beginPoint();
	for(int i = 0; i < numV; i++) 
	{
		const Vertex p = poly->getVertex(i);
		aVertex(p.x, p.y, p.z);
	}
	end();
}

void ShapeDrawer::drawWiredFace(const Polytode * poly)
{
	beginLine();
	
	const int numFace = poly->getNumFace();
	
	for(int i = 0; i < numFace; i++ )
	{
		const Facet f = poly->getFacet(i);

		Vertex p0 = f.getVertex(0);
		Vertex p1 = f.getVertex(1);
		Vertex p2 = f.getVertex(2);
		
		aVertex(p0.x, p0.y, p0.z);
		aVertex(p1.x, p1.y, p1.z);
		
		aVertex(p1.x, p1.y, p1.z);
		aVertex(p2.x, p2.y, p2.z);
		
		aVertex(p2.x, p2.y, p2.z);
		aVertex(p0.x, p0.y, p0.z);
	}
	
	end();
}

void ShapeDrawer::drawNormal(const Polytode * poly)
{
	const int numFace = poly->getNumFace();
	beginLine();
	for(int i = 0; i < numFace; i++ )
	{
		const Facet f = poly->getFacet(i);

		const Vector3F c = f.getCentroid();
		const Vector3F nor = f.getNormal();
		aVertex(c.x, c.y, c.z);
		aVertex(c.x + nor.x, c.y + nor.y, c.z + nor.z);
	}
	end();
}

