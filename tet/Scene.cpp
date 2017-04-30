/*
 *  Scene.cpp
 *  
 *
 *  Created by jian zhang on 6/1/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#include "Scene.h"
using namespace aphid;
namespace ttg {

Scene::Scene() {}
Scene::~Scene() {}

bool Scene::init() 
{ return true; }

bool Scene::progressForward()
{ return true; }

bool Scene::progressBackward()
{ return true; }

const char * Scene::titleStr() const
{ return "unknown"; }

void Scene::draw(GeoDrawer * dr) {}

bool Scene::viewPerspective() const
{ return false; }

void Scene::setView(const PerspectiveView * f)
{ m_view = f; }

const PerspectiveView * Scene::view() const
{ return m_view; }

}