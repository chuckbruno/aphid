/*
 *  PlantSelection.cpp
 *  proxyPaint
 *
 *  Created by jian zhang on 1/31/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#include "PlantSelection.h"

namespace aphid {

PlantSelection::PlantSelection(sdb::WorldGrid<sdb::Array<int, Plant>, Plant > * grid)
{ 
	m_grid = grid; 
	m_plants = new sdb::Array<int, PlantInstance>();
	m_numSelected = 0;
    m_radius = 8.f;
    m_typeFilter = -1;
}

PlantSelection::~PlantSelection()
{ delete m_plants; }
	
void PlantSelection::setRadius(float x)
{ m_radius = (x > 0.1f) ? x : 0.1f; }

void PlantSelection::setCenter(const Vector3F & center, const Vector3F & direction)
{
	m_center = center;
	m_direction = direction;
}

void PlantSelection::select(SelectionContext::SelectMode mode)
{
	int ng = 1 + m_radius / m_grid->gridSize();
	
	const sdb::Coord3 c0 = m_grid->gridCoord((const float *)&m_center);
	sdb::Coord3 c1;
	int i, j, k;
	for(k=-ng; k<=ng; ++k) {
		c1.z = c0.z + k;
		for(j=-ng; j<=ng; ++j) {
			c1.y = c0.y + j;
			for(i=-ng; i<=ng; ++i) {
				c1.x = c0.x + i;
				select(c1, mode);
			}
		}
	}
	m_numSelected = m_plants->size();
}

void PlantSelection::select(const sdb::Coord3 & c, 
                        const SelectionContext::SelectMode & mode)
{
	BoundingBox b = m_grid->coordToGridBBox(c);
	if(!b.isPointAround(m_center, m_radius) ) return;
	sdb::Array<int, Plant> * cell = m_grid->findCell(c);
	if(!cell) return;
	if(cell->isEmpty() ) return;
	
	SelectionContext::SelectMode usemode = mode;
	cell->begin();
	while(!cell->end()) {
		PlantData * d = cell->value()->index;
		if(m_center.distanceTo(d->t1->getTranslation() ) < m_radius) {
            if(m_typeFilter > -1 ) {
                if(m_typeFilter == *d->t3) 
                    usemode = mode;
                else
                    usemode = SelectionContext::Unknown;
                    
            }
			if(usemode == SelectionContext::Append) select(cell->value() );
			else if(usemode == SelectionContext::Remove) m_plants->remove(cell->key() );
		}
		
		cell->next();
	}
}

void PlantSelection::selectByType(int x)
{
	if(m_grid->isEmpty() ) return;
	m_grid->begin();
	while(!m_grid->end() ) {
		selectByTypeInCell(m_grid->value(), x);
		m_grid->next();
	}
	m_numSelected = m_plants->size();
}

void PlantSelection::selectByTypeInCell(sdb::Array<int, Plant> * cell, int x)
{
	if(cell->isEmpty() ) return;
	cell->begin();
	while(!cell->end()) {
		PlantData * d = cell->value()->index;
		if(x == *d->t3) select(cell->value() );
		
		cell->next();
	}
}

void PlantSelection::select(Plant * p)
{ 
	PlantData * backup = new PlantData;
	*backup->t1 = *p->index->t1;
	*backup->t2 = *p->index->t2;
	*backup->t3 = *p->index->t3;
	
	Plant * b = new Plant;
	b->key = p->key;
	b->index = backup;
	
	PlantInstance * inst = new PlantInstance;
	inst->m_backup = b;
	inst->m_reference = p;
	m_plants->insert(p->key, inst);
}

void PlantSelection::deselect()
{ 
	m_plants->clear();
	m_numSelected = 0;
}

void PlantSelection::updateNumSelected()
{ m_numSelected = m_plants->size(); }

const unsigned & PlantSelection::numSelected() const
{ return m_numSelected; }
	
sdb::Array<int, PlantInstance> * PlantSelection::data()
{ return m_plants; }

void PlantSelection::calculateWeight()
{
	m_plants->begin();
	while(!m_plants->end() ) {
		const Vector3F pr = m_plants->value()->m_reference->index->t1->getTranslation();
		const float dist = m_center.distanceTo(pr);
		if(dist < m_radius) {
			m_plants->value()->m_weight = 1.f - sqrt(dist / m_radius);
		}
		else {
			m_plants->value()->m_weight = 0.f;
		}
		m_plants->next();
	}
}

const float & PlantSelection::radius() const
{ return m_radius; }

void PlantSelection::setTypeFilter(int x)
{ m_typeFilter = x; }

bool PlantSelection::touchCell(const Ray & incident, const sdb::Coord3 & c, 
								Vector3F & pnt)
{
	sdb::Array<int, Plant> * cell = m_grid->findCell(c);
	if(!cell) return false;
	if(cell->isEmpty() ) return false;
	float tt;
	cell->begin();
	while(!cell->end()) {
		PlantData * d = cell->value()->index;
		pnt = incident.closetPointOnRay(d->t1->getTranslation(), &tt );
		if(tt < -1.f 
			&& pnt.distanceTo(d->t1->getTranslation() ) < m_radius)
            return true;
		
		cell->next();
	}
	return false;
}

}