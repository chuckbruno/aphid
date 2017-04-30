/*
 *  SceneContainer.cpp
 *  qtbullet
 *
 *  Created by jian zhang on 7/13/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "SceneContainer.h"
#include <AllMath.h>
#include <RandomMesh.h>
#include <KdCluster.h>
#include <KdTreeDrawer.h>
#include <BezierCurve.h>
#include <CurveBuilder.h>
#include <GeometryArray.h>
#include <RandomCurve.h>
#include <bezierPatch.h>
#include <Ray.h>

#define TEST_CURVE 0
#define TEST_MESH 1
#define NUM_CURVESU 20
#define NUM_CURVESV 20
#define NUM_CURVES 400

SceneContainer::SceneContainer(GeoDrawer * dr) 
{
	m_geoDrawer = dr;
	m_intersectCtx.m_success = 0;
	
	m_level = 28;
	m_drawer = new KdTreeDrawer;
	m_cluster = new KdCluster;
	
	sdb::TreeNode::MaxNumKeysPerNode = 128;
    sdb::TreeNode::MinNumKeysPerNode = 16;
	
#if TEST_MESH
	unsigned i=0;
	float up = .2f, yb = 0.f;
	for(;i<NUM_MESHES;++i) {
		if(i==NUM_MESHES/2) {
			up = -.4f;
			yb = NUM_MESHES * .2f;
		}
		Vector3F c(-10.f + i * .1f + 75.f * RandomF01(), 
					yb + up * i + 42.f * RandomF01(), 
					-1.f * i + -30.f + 45.f * RandomF01());
		m_mesh[i] = new RandomMesh(12500 - 1200 * RandomF01(), c, 10.f - 3.f * RandomF01(), i&1);
	}
	testMesh();
#endif

#if TEST_CURVE
	testCurve();
#endif
}

SceneContainer::~SceneContainer() 
{ delete m_drawer; }

void SceneContainer::testMesh()
{
	m_tree = new KdTree;
	unsigned i=0;
	for(;i<NUM_MESHES;++i)
		m_tree->addGeometry(m_mesh[i]);
	
	TreeProperty::BuildProfile bf;
	bf._maxLeafPrims = 1024;
	bf._maxLevel = m_level;
	m_tree->create(&bf);
}

void SceneContainer::testCurve()
{
	m_curves = new GeometryArray;
	m_curves->create(NUM_CURVES);
	
	BezierPatch bp;
	bp.resetCvs();
	
	int i=0;
	bp._contorlPoints[0].y += -.2f;
	bp._contorlPoints[1].y += -.4f;
	bp._contorlPoints[2].y += -.4f;
	bp._contorlPoints[3].y += -.5f;
	
	bp._contorlPoints[4].y += -.5f;
	
	bp._contorlPoints[7].y += .1f;
	
	bp._contorlPoints[9].y += .5f;
	bp._contorlPoints[10].y += .5f;
	
	bp._contorlPoints[13].y += -.4f;
	bp._contorlPoints[14].y += -.85f;
	bp._contorlPoints[15].y += -.21f;
	
	i=0;
	for(;i<16;i++) {
		bp._contorlPoints[i] *= 60.f;
		bp._contorlPoints[i].y -= 10.f;
		bp._contorlPoints[i].z -= 10.f;
	}
	
	RandomCurve rc;
	rc.create(m_curves, NUM_CURVESU, NUM_CURVESV,
				&bp,
				Vector3F(-.15f, 1.f, 0.33f), 
				11, 21,
				.9f);

	m_cluster->addGeometry(m_curves);
	m_cluster->create();
}

void SceneContainer::renderWorld()
{
	m_geoDrawer->setGrey(.3f);
	m_geoDrawer->setWired(0);
	int i=0;
	
#if TEST_MESH
	for(;i<NUM_MESHES;i++) 
		m_geoDrawer->triangleMesh(m_mesh[i]);
		
	drawIntersection();
	drawClosest();
#endif	
	glColor3f(0.1f, .2f, .3f);
	
#if TEST_CURVE
	// for(i=0; i<NUM_CURVES; i++)
		// m_drawer->smoothCurve(*(BezierCurve *)m_curves->geometry(i), 4);
	// glColor3f(.354f,.8333f,.12f);
	for(i=0; i<m_cluster->numGroups(); i++) {
		m_geoDrawer->setGroupColorLight(i);
		m_geoDrawer->geometry(m_cluster->group(i));
	}
#endif
		
	m_geoDrawer->setWired(1);
	m_geoDrawer->setColor(.15f, .6f, .5f);
#if TEST_MESH
	m_geoDrawer->boundingBox(m_tree->getBBox() );
	m_drawer->drawKdTree(m_tree);
#endif

#if TEST_CURVE
	m_drawer->drawKdTree(m_cluster);
#endif
}

void SceneContainer::upLevel()
{
	m_level++;
	if(m_level > 30) m_level = 30;
#if TEST_MESH
	//m_tree->rebuild();
	delete m_tree;
	testMesh();
#endif
#if TEST_CURVE
	m_cluster->rebuild();
#endif
}

void SceneContainer::downLevel()
{
	m_level--;
	if(m_level<2) m_level = 2;
#if TEST_MESH
	//m_tree->rebuild();
	delete m_tree;
	testMesh();
#endif
#if TEST_CURVE
	m_cluster->rebuild();
#endif
}

void SceneContainer::intersect(const Ray * incident)
{
	m_intersectCtx.reset(*incident);
#if TEST_MESH
std::cout<<"\n intersect begin";
	m_tree->intersect(&m_intersectCtx );
	if(m_intersectCtx.m_success) {
		m_selectCtx.setRadius(1.f);
		m_selectCtx.setSelectMode(SelectionContext::Append);
		m_selectCtx.setCenter(m_intersectCtx.m_hitP);
		m_selectCtx.setDirection(m_intersectCtx.m_hitN);

		m_tree->select(&m_selectCtx);
	}
std::cout<<"\n intersect end";
#endif
}

void SceneContainer::drawIntersection()
{
#if TEST_MESH
	glColor3f(0,1,0);
	glBegin(GL_TRIANGLES);
	std::map<Geometry *, sdb::Sequence<unsigned> * >::iterator it = m_selectCtx.geometryBegin();
	for(;it!=m_selectCtx.geometryEnd();++it) {
		ATriangleMesh * mesh = static_cast<ATriangleMesh *>(it->first);
		const Vector3F * p = mesh->points();
		
		sdb::Sequence<unsigned> * cell = it->second;
		cell->begin();
		while(!cell->end() ) {
			unsigned component = cell->key();
			unsigned * tri = mesh->triangleIndices(component);
			glVertex3fv((const GLfloat * )&p[tri[0] ]);
			glVertex3fv((const GLfloat * )&p[tri[1] ]);
			glVertex3fv((const GLfloat * )&p[tri[2] ]);
			cell->next();
		}
	}
	glEnd();
#endif
}

void SceneContainer::drawClosest()
{
#if TEST_MESH
	if(!m_intersectCtx.m_success) return;
	
	Vector3F orig = m_intersectCtx.m_hitP;
	orig.x += 20.f;
	orig.y -= 20.f;
	
	m_closestPointTest.reset(orig, orig.distanceTo( m_intersectCtx.m_hitP));
	m_tree->closestToPoint(&m_closestPointTest);
	
	if(m_closestPointTest._hasResult) {
		glColor3f(1,0,0);
		glBegin(GL_LINES);
			glVertex3fv((const GLfloat * )&orig);
			glVertex3fv((const GLfloat * )&m_closestPointTest._hitPoint);
		glEnd();
	}
	
#endif
}
