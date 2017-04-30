/*
 *  Edge.cpp
 *  convexHull
 *
 *  Created by jian zhang on 9/6/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "Edge.h"
#include <topo/Vertex.h>
#include <topo/Facet.h>

namespace aphid {

Edge::Edge() : identicalTwin(0) {}
Edge::~Edge() {}

Edge::Edge(Vertex *a, Vertex *b, char * f)
{
	identicalTwin = 0;
	va = a;
	vb = b;
	face = f;
}

char Edge::equals(int i, int j) const
{
	return ((va->getIndex() == i && vb->getIndex() == j));
}

char Edge::matches(Edge *e) const
{
	return matches(e->v0(), e->v1());
}

char Edge::matches(Vertex *a, Vertex *b) const
{
	//return ((va->equals(*a) && vb->equals(*b)) ||
    //          (va->equals(*b) && vb->equals(*a)));
	return ((va->getIndex() == a->getIndex() && vb->getIndex() == b->getIndex()) ||
              (va->getIndex() == b->getIndex() && vb->getIndex() == a->getIndex()));
}

char Edge::isOppositeOf(int i, int j) const
{
	return ((va->getIndex() == j && vb->getIndex() == i));
}

char Edge::isOppositeOf(Edge *e) const
{
	return ((va->getIndex() == e->v1()->getIndex() && vb->getIndex() == e->v0()->getIndex()));
}

void Edge::setTwin(Edge *e)
{
	identicalTwin = e;
}

Edge * Edge::getTwin() const
{
	return identicalTwin;
}

char * Edge::getFace() const
{
	return face;
}

Vertex *Edge::v0()
{
	return va;
}

Vertex *Edge::v1()
{
	return vb;
}

Vertex Edge::getV0() const
{
	return *va;
}

Vertex Edge::getV1() const
{
	return *vb;
}

char Edge::canBeConnectedTo(Edge * another) const
{
	if(matches(another->v0(), another->v1())) return 0;
	if(another->v0()->getIndex() == vb->getIndex() || another->v1()->getIndex() == vb->getIndex()) return 1;
	return 0;
}

void Edge::connect(Edge * another)
{
	next = another;
	if(another->v0()->getIndex() == va->getIndex() || another->v1()->getIndex() == vb->getIndex()) another->flip();
}

void Edge::flip()
{
	Vertex *t = va;
	va = vb;
	vb = t;
}

void Edge::disconnect()
{
	next = 0;
}

char Edge::isReal() const
{
	Edge *opp = getTwin();
	if(!opp) return 1;
	
	Facet *f0 = (Facet *)getFace();
	Facet *f1 = (Facet *)opp->getFace();

	return f0->getPolygonIndex() != f1->getPolygonIndex();
}

char Edge::connectedToVertex(unsigned idx) const
{
	if(va->getIndex() == idx)
		return 1;
	if(vb->getIndex() == idx)
		return 1;
	return 0;
}

}