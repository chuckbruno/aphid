/*
 *  ManipulateView.cpp
 *  fit
 *
 *  Created by jian zhang on 5/6/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */
#include <QtGui>
#include <QtOpenGL>
#include <math.h>
#include "ManipulateView.h"
#include <KdTreeDrawer.h>
#include <PatchMesh.h>
#include <KdTree.h>
#include <Ray.h>
#include <ToolContext.h>
#include <TransformManipulator.h>
#include <MeshManipulator.h>
#include <BaseBrush.h>

ManipulateView::ManipulateView(QWidget *parent) : Base3DView(parent)
{
	m_tree = new KdTree;
	m_manipulator = new TransformManipulator;
	m_sculptor = new MeshManipulator;
	m_shouldRebuildTree = false;
	m_selectCtx = new SelectionContext;
}
//! [0]

//! [1]
ManipulateView::~ManipulateView()
{
}

void ManipulateView::clientDraw()
{
	getDrawer()->drawKdTree(m_tree);
}
//! [7]

//! [9]
void ManipulateView::clientSelect(QMouseEvent *event)
{
	if(isSelectingComponent()) selectComponent(event);
	if(isTransforming()) startTransform(event);
}
//! [9]

void ManipulateView::clientDeselect(QMouseEvent *event) 
{
	if(isTransforming()) endTransform();
}

void ManipulateView::clientMouseInput(QMouseEvent *event)
{
	if(isSelectingComponent()) selectComponent(event);
	if(isTransforming()) doTransform(event);
}

Vector3F ManipulateView::sceneCenter() const
{
	if(m_tree->isEmpty()) return Base3DView::sceneCenter();
	return m_tree->getBBox().center();
}

bool ManipulateView::pickupComponent(const Ray & ray, Vector3F & hit)
{
	getIntersectionContext()->reset(ray);
	if(!m_tree->intersect(getIntersectionContext())) 
		return false;
	hit = getIntersectionContext()->m_hitP;
	addHitToSelection();
	return true;
}

bool ManipulateView::hitTest()
{
	Ray ray = *getIncidentRay();
	getIntersectionContext()->reset(ray);
	return m_tree->intersect(getIntersectionContext());
}

void ManipulateView::buildTree()
{
	if(!activeMesh()) return;
	if(m_tree) delete m_tree;
	m_tree = new KdTree;
	m_tree->addMesh(activeMesh());
	m_tree->create();
}

void ManipulateView::focusInEvent(QFocusEvent * event)
{
	if(m_shouldRebuildTree) {
		buildTree();
		m_shouldRebuildTree = false;
	}
	Base3DView::focusInEvent(event);
}

void ManipulateView::setRebuildTree()
{
	m_shouldRebuildTree = true;
}

bool ManipulateView::shouldRebuildTree() const
{
	return m_shouldRebuildTree;
}

KdTree * ManipulateView::getTree() const
{
	return m_tree;
}

PatchMesh * ManipulateView::activeMesh() const
{
	return NULL;
}

void ManipulateView::drawIntersection() const
{
    IntersectionContext * ctx = getIntersectionContext();
    if(!ctx->m_success) return;
    
	getDrawer()->drawPrimitivesInNode(m_tree, (KdTreeNode *)ctx->m_cell);
	
	Base3DView::drawIntersection();
}

TransformManipulator * ManipulateView::manipulator()
{
	return m_manipulator;
}

MeshManipulator * ManipulateView::sculptor()
{
    return m_sculptor;
}

void ManipulateView::showManipulator() const
{
	getDrawer()->manipulator(m_manipulator);
}

void ManipulateView::keyPressEvent(QKeyEvent *e)
{	
	switch (e->key()) {
		case Qt::Key_T:
			manipulator()->setToMove();
			break;
		case Qt::Key_R:
			manipulator()->setToRotate();
			break;
		default:
			break;
	}
		
	Base3DView::keyPressEvent(e);
}

void ManipulateView::clearSelection()
{
	m_selectCtx->discard();
	m_manipulator->detach();
	Base3DView::clearSelection();
}

const std::deque<unsigned> & ManipulateView::selectedQue() const
{
	return m_selectCtx->selectedQue();
}

bool ManipulateView::isSelectingComponent() const
{
	return interactMode() == ToolContext::SelectFace;
}

void ManipulateView::selectComponent(QMouseEvent *event)
{
	SelectionContext::SelectMode selm = SelectionContext::Replace;
	switch(event->modifiers()) {
		case Qt::ShiftModifier:
			selm = SelectionContext::Append;
			break;
#ifdef WIN32
		case Qt::ControlModifier:
#else
		case Qt::MetaModifier:
#endif
			selm = SelectionContext::Remove;
			break;
		default:
			break;
	}
	
	switch (interactMode()) {
		case ToolContext::SelectFace :
			hitTest();
			selectFaces(selm);
			break;
		default:
			break;
	}
}

void ManipulateView::selectFaces(SelectionContext::SelectMode m)
{
	IntersectionContext * ctx = getIntersectionContext();
    if(!ctx->m_success) return;
	
	brush()->setSpace(ctx->m_hitP, ctx->m_hitN);
	brush()->resetToe();
	
	m_selectCtx->setSelectMode(m);
	m_selectCtx->reset(ctx->m_hitP, brush()->getRadius() + brush()->minDartDistance());
	if(!brush()->twoSided()) m_selectCtx->setDirection(ctx->m_hitN);
	m_tree->select(m_selectCtx);
	m_selectCtx->finish();
}

bool ManipulateView::isTransforming() const
{
	return interactMode() == ToolContext::MoveTransform || interactMode() == ToolContext::RotateTransform;
}

void ManipulateView::startTransform(QMouseEvent *event)
{
	if(manipulator()->isDetached()) return;
	switch (interactMode()) {
		case ToolContext::MoveTransform :
			manipulator()->setToMove();
			break;
		case ToolContext::RotateTransform :
			manipulator()->setToRotate();
			break;
	    default:
			break;
	}
	
	switch (event->button()) {
		case Qt::LeftButton:
			manipulator()->setRotateAxis(TransformManipulator::AY);
			break;
		case Qt::MiddleButton:
			manipulator()->setRotateAxis(TransformManipulator::AZ);
			break;
		default:
			manipulator()->setRotateAxis(TransformManipulator::AX);
			break;
	}
	
	Ray ray = *getIncidentRay();
	manipulator()->start(&ray);
}

void ManipulateView::doTransform(QMouseEvent *)
{
	if(manipulator()->isDetached()) return;
	Ray ray = *getIncidentRay();
	manipulator()->perform(&ray);
}

void ManipulateView::endTransform()
{
	manipulator()->stop();
	manipulator()->detach();
}

void  ManipulateView::showActiveFaces() const
{
	getDrawer()->m_wireProfile.apply();
	getDrawer()->useDepthTest(0);
	getDrawer()->colorAsReference();
	getDrawer()->patch(activeMesh(), selectedQue());
	getDrawer()->useDepthTest(1);
}
//:~