#include "CudaDynamicWorld.h"
#include <CudaBroadphase.h>
#include <CudaNarrowphase.h>
#include <SimpleContactSolver.h>
#include <CudaTetrahedronSystem.h>
#include <CudaBase.h>
#include <DrawBvh.h>

CudaDynamicWorld::CudaDynamicWorld() 
{
    m_broadphase = new CudaBroadphase;
    m_narrowphase = new CudaNarrowphase;
	m_contactSolver = new SimpleContactSolver;
	m_numObjects = 0;
	
	m_dbgBvh = new DrawBvh;
}

CudaDynamicWorld::~CudaDynamicWorld()
{
    delete m_broadphase;
    delete m_narrowphase;
    delete m_contactSolver;
}

const unsigned CudaDynamicWorld::numObjects() const
{ return m_numObjects; }

void CudaDynamicWorld::setDrawer(GeoDrawer * drawer)
{ m_dbgBvh->setDrawer(drawer); }

void CudaDynamicWorld::addTetrahedronSystem(CudaTetrahedronSystem * tetra)
{
    if(m_numObjects == CUDA_DYNAMIC_WORLD_MAX_NUM_OBJECTS) return;
    
    m_objects[m_numObjects] = tetra;
    m_numObjects++;
    
    m_broadphase->addBvh(tetra);
    m_narrowphase->addTetrahedronSystem(tetra);
}

void CudaDynamicWorld::initOnDevice()
{
    if(m_numObjects < 1) return;
    CudaBase::SetDevice();
    
    unsigned i;
	for(i=0; i < m_numObjects; i++) m_objects[i]->initOnDevice();
	
    m_broadphase->initOnDevice();
    m_narrowphase->initOnDevice();
	m_contactSolver->initOnDevice();
}

void CudaDynamicWorld::stepPhysics(float dt)
{
    if(m_numObjects < 1) return;
	unsigned i;
	for(i=0; i < m_numObjects; i++) m_objects[i]->update();
	m_broadphase->computeOverlappingPairs();
	
	m_dbgBvh->showOverlappingPairs(m_broadphase);
	
	for(i=0; i < m_numObjects; i++) m_objects[i]->integrate(dt);
}

