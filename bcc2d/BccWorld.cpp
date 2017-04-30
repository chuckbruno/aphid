#include "BccWorld.h"
#include <BezierCurve.h>
#include <KdTreeDrawer.h>
#include <CurveGroup.h>
#include <HesperisFile.h>
#include <KdCluster.h>
#include <KdIntersection.h>
#include <GeometryArray.h>
#include <CurveBuilder.h>
#include <BezierCurve.h>
#include <RandomCurve.h>
#include <bezierPatch.h>
#include <ATriangleMesh.h>
#include "FitBccMeshBuilder.h"
#include "CurveReduction.h"
#include <ATetrahedronMeshGroup.h>
#include <BlockBccMeshBuilder.h>
#include "SingleMeshBuilder.h"

BccWorld::BccWorld()
{
    m_numCurves = 0;
	m_numPatches = 0;
	m_totalCurveLength = 0.f;
	m_totalPatchArea = 0.f;
    m_estimatedNumGroups = 1000.f;
	m_triangleMeshes = NULL;
	m_reducer = new CurveReduction;
	m_blockBuilder = new BlockBccMeshBuilder;
	m_fitBuilder = new FitBccMeshBuilder;
    m_singleBuilder = new SingleMeshBuilder;
	m_curveCluster = NULL;
	m_patchCluster = NULL;
    m_patchGenMethod = MBlock;
}

BccWorld::~BccWorld() 
{
    delete m_reducer;
	delete m_curveCluster;
	delete m_patchCluster;
	delete m_triangleMeshes;
	delete m_blockBuilder;
	delete m_fitBuilder;
    delete m_singleBuilder;
}

void BccWorld::setTiangleGeometry(GeometryArray * x)
{ m_triangleMeshes = x; }

void BccWorld::addCurveGroup(CurveGroup * m)
{ m_curveGeos.push_back(m); }

GeometryArray * BccWorld::selectedGroup(unsigned & idx) const
{
	if(!m_curveCluster) return NULL;
	idx = m_curveCluster->currentGroup();
	if(!m_curveCluster->isGroupIdValid(idx)) return NULL;
	return m_curveCluster->group(idx);
}

float BccWorld::drawAnchorSize() const
{ return FitBccMeshBuilder::EstimatedGroupSize / 6.f; }

const float BccWorld::totalCurveLength() const
{ return m_totalCurveLength; }

const unsigned BccWorld::numCurves() const
{ return m_numCurves; }

unsigned BccWorld::numPatches() const
{ return m_numPatches; }

const unsigned BccWorld::numTetrahedrons() const
{ return m_totalNumTetrahedrons; }

const unsigned BccWorld::numPoints() const
{ return m_totalNumPoints; }

ATetrahedronMeshGroup * BccWorld::tetrahedronMesh(unsigned i) const
{ return m_tetrahedonMeshes[i]; }

void BccWorld::clearTetrahedronMesh()
{ 
	std::vector<ATetrahedronMeshGroup *>::iterator it = m_tetrahedonMeshes.begin();
	for(;it!=m_tetrahedonMeshes.end();++it) delete *it;
	m_tetrahedonMeshes.clear(); 
}

unsigned BccWorld::numTetrahedronMeshes() const
{ return m_tetrahedonMeshes.size(); }

GeometryArray * BccWorld::triangleGeometries() const
{ return m_triangleMeshes; }

unsigned BccWorld::numTriangles() const
{
	if(!m_triangleMeshes) return 0;
	const unsigned m = m_triangleMeshes->numGeometries();
	unsigned nt = 0;
	unsigned i = 0;
	for(;i<m;i++) nt += ((ATriangleMesh *)m_triangleMeshes->geometry(i))->numTriangles();
	return nt;
}

void BccWorld::rebuildTetrahedronsMesh(float deltaNumGroups)
{
    m_estimatedNumGroups += deltaNumGroups * (numCurves() + numPatches() * 2);
    rebuildTetrahedronsMeshBy(m_estimatedNumGroups);
}
 
void BccWorld::rebuildTetrahedronsMeshBy(unsigned n)
{
    if(n < 100.f) n = 100.f;
    m_estimatedNumGroups = n;
    clearTetrahedronMesh();
	buildTetrahedronMesh(false);
    std::cout<<" done rebuild. \n";
}

void BccWorld::select(const Ray * r)
{
	if(!m_curveCluster) return;
	BaseCurve::RayIntersectionTolerance = totalCurveLength() / m_estimatedNumGroups * .37f;
	if(!m_curveCluster->intersectRay(r))
		clearSelection();
}

void BccWorld::clearSelection()
{ m_curveCluster->setCurrentGroup(m_curveCluster->numGroups()); }

float BccWorld::groupCurveLength(GeometryArray * geos)
{
	float sum = 0.f;
	const unsigned n = geos->numGeometries();
     unsigned i = 0;
    for(;i<n;i++) {
        BezierCurve * c = static_cast<BezierCurve *>(geos->geometry(i));
        sum += c->length();
    }
	return sum;
}

void BccWorld::reduceSelected(float x)
{
    const unsigned selectedCurveGrp = m_curveCluster->currentGroup();
	if(m_curveCluster->isGroupIdValid(selectedCurveGrp))
		reduceGroup(selectedCurveGrp);
	else
        reduceAllGroups();	
}

void BccWorld::reduceAllGroups()
{
	unsigned i = 0;
	for(;i<m_curveCluster->numGroups();i++) reduceGroup(i);
}

void BccWorld::reduceGroup(unsigned igroup)
{
	const unsigned oldNCurves = m_curveCluster->group(igroup)->numGeometries();
    GeometryArray * reduced = 0;
	
	int i=0;
	for(;i<20;i++) {
		GeometryArray * ir = m_reducer->compute(m_curveCluster->group(igroup), FitBccMeshBuilder::EstimatedGroupSize);
		if(ir) {
			reduced = ir;
			m_curveCluster->setGroupGeometry(igroup, reduced);
		}
		else break;
	}
	
	if(!reduced) {
        std::cout<<" insufficient condition for curve reduction, skipped.\n";
        return;
    }
	
	m_numCurves -= oldNCurves;
	m_numCurves += reduced->numGeometries();
	
	rebuildGroupTetrahedronMesh(igroup, reduced);
}

void BccWorld::rebuildGroupTetrahedronMesh(unsigned igroup, GeometryArray * geos)
{	
	ATetrahedronMeshGroup * amesh = m_tetrahedonMeshes[igroup];
	const unsigned oldNVert = amesh->numPoints();
	const unsigned oldNTet = amesh->numTetrahedrons();
	const unsigned oldTotalNTet = m_totalNumTetrahedrons;
	const unsigned oldTotalNVert = m_totalNumPoints;
	
	delete amesh;
	
	m_tetrahedonMeshes[igroup] = genTetFromGeometry(geos, m_fitBuilder);
	
	m_totalNumPoints -= oldNVert;
	m_totalNumPoints += m_tetrahedonMeshes[igroup]->numPoints();
	m_totalNumTetrahedrons -= oldNTet;
	m_totalNumTetrahedrons += m_tetrahedonMeshes[igroup]->numTetrahedrons();
	
	std::cout<<" reduce n points from "<<oldTotalNTet<<" to "<<m_totalNumPoints
	<<"\n n tetrahedrons form "<<oldTotalNTet<<" to "<<m_totalNumTetrahedrons
	<<"\n";
}

void BccWorld::addCurveGeometriesToCluster(CurveGroup * data)
{
	unsigned * cc = data->counts();
	Vector3F * cvs = data->points();
	const unsigned n = data->numCurves();
	
	GeometryArray * geos = new GeometryArray;
	geos->create(n);
	
	CurveBuilder cb;
	
	unsigned ncv;
	unsigned cvDrift = 0;
	
	unsigned i, j;
	for(i=0; i< n; i++) {
		
		ncv = cc[i];
		
		for(j=0; j < ncv; j++)			
			cb.addVertex(cvs[j + cvDrift]);
		
		BezierCurve * c = new BezierCurve;
		cb.finishBuild(c);
		
		geos->setGeometry(c, i);
		
		cvDrift += ncv;
	}
	
	m_curveCluster->addGeometry(geos);
}

bool BccWorld::createTriangleIntersection()
{	
	if(!m_triangleMeshes) return false;
	unsigned i;
	m_triIntersect = new KdIntersection;
	for(i=0; i<m_triangleMeshes->numGeometries(); i++) {
		ATriangleMesh * m = (ATriangleMesh *)m_triangleMeshes->geometry(i);
		std::cout<<"\n mesh["<<i<<"] n triangles: "<<m->numTriangles()
		<<"\n n points: "<<m->numPoints()
		<<"\n";
		m_triIntersect->addGeometry(m);
	}
	
	KdTree::MaxBuildLevel = 20;
	KdTree::NumPrimitivesInLeafThreashold = 9;
	m_triIntersect->create();
	return true;
}

bool BccWorld::buildTetrahedronMesh(bool reset)
{	
	if(!createTriangleIntersection()) {
		std::cout<<"\n bcc world has no grow mesh ";
		return false;
	}
	
	if(reset) {
// force re-clustering
		createCurveCluster();
		createPatchCluster();
	}
	else {
		if(!m_curveCluster) createCurveCluster();
		if(!m_patchCluster) createPatchCluster();
	}
	
	m_totalCurveLength = computeTotalCurveLength();
	m_totalPatchArea = computeTotalPatchArea();
	std::cout<<"\n total curve length: "<<m_totalCurveLength
			<<"\n total patch area: "<<m_totalPatchArea;
			
	FitBccMeshBuilder::EstimatedGroupSize = (m_totalCurveLength + m_totalPatchArea) / m_estimatedNumGroups;
    std::cout<<"\n estimate group size "<<FitBccMeshBuilder::EstimatedGroupSize;

	createTetrahedronMeshesByFitCurves();
	createTetrahedronMeshesByBlocks();
	
	unsigned ntet, nvert, nanchor, nstripe;
	computeTetrahedronMeshStatistics(ntet, nvert, nstripe, nanchor);
	
	std::cout<<"\n n tetrahedron meshes "<<numTetrahedronMeshes()
	<<"\n total n tetrahedrons "<<ntet
	<<"\n total n points "<<nvert
	<<"\n total n anchored points "<<nanchor
    <<"\n total n stripes "<<nstripe
	<<"\n building finished \n";
	m_totalNumTetrahedrons = ntet;
	m_totalNumPoints = nvert;
	return true;
}

float BccWorld::computeTotalCurveLength()
{
	if(!m_curveCluster) return 0.f;
	const unsigned n = m_curveCluster->numGroups();
	float sum = 0.f;
	unsigned i;
	for(i=0; i< n; i++)
		sum += groupCurveLength(m_curveCluster->group(i));
		
	return sum;
}

float BccWorld::computeTotalPatchArea()
{
	if(!m_patchCluster) return 0.f;
	const unsigned n = m_patchCluster->numGroups();
	float sum = 0.f;
	unsigned i;
	for(i=0;i<n;i++)
		sum += groupPatchArea(m_patchCluster->group(i));
	return sum;
}

float BccWorld::groupPatchArea(GeometryArray * geos)
{
	const unsigned n = geos->numGeometries();
	float sum = 0.f;
	Vector3F patchExtent;
	unsigned i = 0;
    for(;i<n;i++) {
        AOrientedBox * b = static_cast<AOrientedBox *>(geos->geometry(i));
		patchExtent = b->extent();
        sum += patchExtent.x * patchExtent.y * 4.f;
    }
	return sum;
}
	
bool BccWorld::createCurveCluster()
{
	const unsigned n = m_curveGeos.size();
	if(n < 1) return false;
	
	std::cout<<"\n bcc world creating curve cluster ";
	if(m_curveCluster) delete m_curveCluster;
	m_curveCluster = new KdCluster;
	
	m_numCurves = 0;
	
	unsigned i;
	for(i=0; i< n; i++) {
		addCurveGeometriesToCluster(m_curveGeos[i]);
		m_numCurves += m_curveGeos[i]->numCurves();
	}
	
	std::cout<<"\n total n input curves "<<m_numCurves;
	
	KdTree::MaxBuildLevel = 6;
	KdTree::NumPrimitivesInLeafThreashold = 31;
	
	m_curveCluster->create();
	return true;
}

void BccWorld::addPatchBoxes(const std::vector<AOrientedBox> & src)
{
	std::vector<AOrientedBox>::const_iterator it = src.begin();
	for(;it!=src.end();++it) m_patchBoxes.push_back(*it);
}

const std::vector<AOrientedBox> * BccWorld::patchBoxes() const
{ return &m_patchBoxes; }

void BccWorld::createTetrahedronMeshesByFitCurves()
{
	if(!m_curveCluster) {
        std::cout<<"\n no curve exists";
		return;
    }
	std::cout<<"\n bcc world building tetrahedron mesh along curve ";

	unsigned n = m_curveCluster->numGroups();
	
	unsigned i;
	for(i=0;i<n;i++)
		m_tetrahedonMeshes.push_back(genTetFromGeometry(m_curveCluster->group(i), m_fitBuilder));
}

ATetrahedronMeshGroup * BccWorld::genTetFromGeometry(GeometryArray * geos, TetrahedronMeshBuilder * builder)
{
	unsigned ntet, nvert, nstripes;
	
	builder->build(geos, ntet, nvert, nstripes);
	
	ATetrahedronMeshGroup * amesh = new ATetrahedronMeshGroup;
	amesh->create(nvert, ntet, nstripes);
	
	builder->getResult(amesh);
	amesh->checkTetrahedronVolume();
	amesh->clearAnchors();
	
	builder->addAnchors(amesh, nstripes, m_triIntersect);
	
	float vlm = amesh->calculateVolume();
	amesh->setVolume(vlm);
	
	return amesh;
}

bool BccWorld::createPatchCluster()
{
	const unsigned n = m_patchBoxes.size();
	if(n < 1) return false;
	
	m_numPatches = n;
	std::cout<<"\n bcc world creating patch cluster ";
	if(m_patchCluster) delete m_patchCluster;
	m_patchCluster = new KdCluster;
	
	GeometryArray * geos = new GeometryArray;
	geos->create(n);
	
	Vector3F patchExtent;
	unsigned i;
	for(i=0; i< n; i++) {
		patchExtent = m_patchBoxes[i].extent();
		geos->setGeometry(&m_patchBoxes[i], i);
	}
	
	m_patchCluster->addGeometry(geos);
	
	std::cout<<"\n total n input patches: "<<n;
	
	KdTree::MaxBuildLevel = 6;
	KdTree::NumPrimitivesInLeafThreashold = 31;
	
	m_patchCluster->create();
	
	return true;
}

void BccWorld::createTetrahedronMeshesByBlocks()
{
	if(!m_patchCluster) {
        std::cout<<"\n no patch exists";
		return;
    }
	unsigned n = m_patchCluster->numGroups();
	unsigned i;
	for(i=0;i<n;i++) {
		// m_tetrahedonMeshes.push_back(genTetFromGeometry(m_patchCluster->group(i), m_blockBuilder));
        m_tetrahedonMeshes.push_back(genTetFromGeometry(m_patchCluster->group(i), m_singleBuilder));
    }
}

void BccWorld::computeTetrahedronMeshStatistics(unsigned & ntet, unsigned & nvert, unsigned & nstripe, unsigned & nanchor) const
{
	ntet = 0;
	nvert = 0;
	nstripe = 0;
	nanchor = 0;
	const unsigned n = numTetrahedronMeshes();
	unsigned i = 0;
	for(; i < n; i++) {
		ATetrahedronMeshGroup * imesh = m_tetrahedonMeshes[i];
		ntet += imesh->numTetrahedrons();
		nvert += imesh->numPoints();
        nstripe += imesh->numStripes();
		nanchor += imesh->numAnchoredPoints();
	}
}

unsigned BccWorld::estimatedNumGroups() const
{ return m_estimatedNumGroups; }

void BccWorld::setPatchGenMethod(int x)
{
    switch (x) {
    case 1:
        m_patchGenMethod = MSingleOctahedron;
        break;
    default:
        m_patchGenMethod = MBlock;
        break;
    }
}
//:~