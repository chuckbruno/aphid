/*
 *  CollisionRegion.cpp
 *  mallard
 *
 *  Created by jian zhang on 9/21/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "CollisionRegion.h"
#include <IntersectionContext.h>
#include <MeshTopology.h>
#include <AccPatchMesh.h>
#include <PatchTexture.h>
#include <BaseSphere.h>

namespace aphid {

CollisionRegion::CollisionRegion() : m_regionElementStart(UINT_MAX) 
{
	m_ctx = new IntersectionContext;
	m_topo = 0;
	m_body = 0;
	m_regionDistribution = 0;
}

CollisionRegion::~CollisionRegion() 
{
	m_regionElementIndices.clear();
	delete m_ctx;
}

void CollisionRegion::clearCollisionRegion()
{
	m_regionElementIndices.clear();
}

AccPatchMesh * CollisionRegion::bodyMesh() const
{
	return m_body;
}

MeshTopology * CollisionRegion::topology() const
{
	return m_topo;
}

void CollisionRegion::setBodyMesh(AccPatchMesh * mesh)
{
	m_body = mesh;
	if(m_topo) delete m_topo;
	m_topo = new MeshTopology(m_body);
	setRegionElementStart(mesh->getNumFaces());
}

Vector3F CollisionRegion::getIntersectPoint(const Vector3F & origin, const Vector3F & dir)
{
	Ray ray(origin, dir);
	m_ctx->reset(ray);
	for(unsigned i=0; i < numRegionElements(); i++) {
		m_body->intersect(regionElementIndex(i), m_ctx);
	}
	Vector3F pos;
	
	m_body->pointOnPatch(m_ctx->m_componentIdx, m_ctx->m_patchUV.x, m_ctx->m_patchUV.y, pos);
	return pos;
}

Vector3F CollisionRegion::getClosestPoint(const Vector3F & origin)
{
	m_ctx->reset();
	closestPoint(origin, m_ctx);
	Vector3F pos;
	m_body->pointOnPatch(m_ctx->m_componentIdx, m_ctx->m_patchUV.x, m_ctx->m_patchUV.y, pos);
	m_hitElement = m_ctx->m_componentIdx;
	/*Vector3F de = pos - m_ctx->m_closestP;
	float er = de.length() / m_body->calculateBBox(m_ctx->m_componentIdx).area() * 2;
	if(er > 0.0001)std::cout<<"\n err "<<er;*/
	return pos;
}

Vector3F CollisionRegion::getClosestNormal(const Vector3F & origin, float maxD, Vector3F & pos)
{
	m_ctx->reset();
	m_ctx->m_minHitDistance = maxD;
	closestPoint(origin, m_ctx);
	Vector3F nor;
	m_body->normalOnPatch(m_ctx->m_componentIdx, m_ctx->m_patchUV.x, m_ctx->m_patchUV.y, nor);
	m_body->pointOnPatch(m_ctx->m_componentIdx, m_ctx->m_patchUV.x, m_ctx->m_patchUV.y, pos);
	m_hitElement = m_ctx->m_componentIdx;
	return nor;
}

void CollisionRegion::neighborFaces(unsigned idx, std::vector<unsigned> & dst)
{
	dst.push_back(idx);
	m_topo->growAroundQuad(idx, dst);
}

void CollisionRegion::resetCollisionRegion(const std::deque<unsigned> & src)
{
	m_regionElementStart = src[0];
	m_regionElementIndices.clear();
	std::deque<unsigned>::const_iterator it = src.begin();
	for(; it != src.end(); ++it) m_regionElementIndices.push_back(*it);
	m_hitElement = -1;
}

void CollisionRegion::resetCollisionRegion(unsigned idx)
{
	m_regionElementStart = idx;
	m_regionElementIndices.clear();
	m_regionElementIndices.push_back(idx);
	m_topo->growAroundQuad(idx, *regionElementIndices());
	m_hitElement = -1;
}

void CollisionRegion::resetCollisionRegionByDistance(unsigned idx, const Vector3F & center, float maxD)
{
    resetCollisionRegion(idx);
	unsigned i, j, lastCreep = 0;
	BoundingBox bb;
	for(i = 1; i < numRegionElements(); i++) {
		bb = m_body->calculateBBox(regionElementIndex(i));
		if(bb.isPointAround(center, maxD))
			lastCreep = m_topo->growAroundQuad(regionElementIndex(i), *regionElementIndices());
		for(j = numRegionElements() - 1 - lastCreep; j < numRegionElements(); j++) {
			bb = m_body->calculateBBox(regionElementIndex(j));
			if(!bb.isPointAround(center, maxD)) {
				regionElementIndices()->erase(regionElementIndices()->begin() + j);
				j--;
			}
		}
	}
}

void CollisionRegion::resetCollisionRegionAround(unsigned idx, const BoundingBox & bbox)
{
    resetCollisionRegion(idx);
	unsigned i, j, lastCreep = 0;
	BoundingBox bb;
	for(i = 1; i < numRegionElements(); i++) {
		bb = m_body->calculateBBox(regionElementIndex(i));
		if(bb.intersect(bbox))
			lastCreep = m_topo->growAroundQuad(regionElementIndex(i), *regionElementIndices());
		for(j = numRegionElements() - 1 - lastCreep; j < numRegionElements(); j++) {
			bb = m_body->calculateBBox(regionElementIndex(j));
			if(!bb.intersect(bbox)) {
				regionElementIndices()->erase(regionElementIndices()->begin() + j);
				j--;
			}
		}
	}
}

void CollisionRegion::resetCollisionRegionAround(unsigned idx, const float & vicinity)
{
    resetCollisionRegion(idx);
    BoundingBox bc = m_body->calculateBBox(idx);
	unsigned i, j, lastCreep = 0;
	BoundingBox bb;
	for(i = 1; i < numRegionElements(); i++) {
		bb = m_body->calculateBBox(regionElementIndex(i));
		if(bb.isBoxAround(bc, vicinity))
			lastCreep = m_topo->growAroundQuad(regionElementIndex(i), *regionElementIndices());
		for(j = numRegionElements() - 1 - lastCreep; j < numRegionElements(); j++) {
			bb = m_body->calculateBBox(regionElementIndex(j));
			if(!bb.isBoxAround(bc, vicinity)) {
				regionElementIndices()->erase(regionElementIndices()->begin() + j);
				j--;
			}
		}
	}
	//std::cout<<"cnf1"<<numRegionElements();
}

void CollisionRegion::resetCollisionRegionAround(unsigned idx, const BaseSphere & sph)
{
    resetCollisionRegion(idx);
	unsigned i, j, lastCreep = 0;
	BoundingBox bb;
	for(i = 1; i < numRegionElements(); i++) {
		bb = m_body->calculateBBox(regionElementIndex(i));
		if(bb.isPointAround(sph.center(), sph.radius()))
			lastCreep = m_topo->growAroundQuad(regionElementIndex(i), *regionElementIndices());
		for(j = numRegionElements() - 1 - lastCreep; j < numRegionElements(); j++) {
			bb = m_body->calculateBBox(regionElementIndex(j));
			if(!bb.isPointAround(sph.center(), sph.radius())) {
				regionElementIndices()->erase(regionElementIndices()->begin() + j);
				j--;
			}
		}
	}
	//std::cout<<"cnf"<<numRegionElements();
}

void CollisionRegion::closestPoint(const Vector3F & origin, IntersectionContext * ctx) const
{
    if(m_hitElement > -1) m_body->closestPoint(m_hitElement, origin, ctx);
	for(unsigned i=0; i < numRegionElements(); i++) {
		m_body->closestPoint(regionElementIndex(i), origin, ctx);
	}
}

void CollisionRegion::pushPlane(Patch::PushPlaneContext * ctx) const
{
	for(unsigned i=0; i < numRegionElements(); i++) {
		m_body->pushPlane(regionElementIndex(i), ctx);
	}
}

unsigned CollisionRegion::numRegionElements() const
{
	return m_regionElementIndices.size();
}

unsigned CollisionRegion::regionElementIndex(unsigned idx) const
{
	return m_regionElementIndices[idx];
}

unsigned CollisionRegion::regionElementStart() const
{
	return m_regionElementStart;
}

void CollisionRegion::setRegionElementStart(unsigned x)
{
	m_regionElementStart = x;
}

std::vector<unsigned> * CollisionRegion::regionElementIndices()
{
	return &m_regionElementIndices;
}

void CollisionRegion::setDistributionMap(PatchTexture * image)
{
    m_regionDistribution = image;
}

void CollisionRegion::selectRegion(unsigned idx, const Vector2F & patchUV)
{
    if(!m_regionDistribution) return;
	if(m_regionDistribution->allWhite()) return;
    m_regionDistribution->sample(idx, patchUV.x, patchUV.y, (float *)&m_sampleColor);
	std::clog<<"select region by color ("<<m_sampleColor.x<<","<<m_sampleColor.y<<","<<m_sampleColor.z<<")\n";
	resetCollisionRegion(idx);
	unsigned i;
	for(i = 1; i < numRegionElements(); i++) {
	    if(faceColorMatches(regionElementIndex(i)))
			m_topo->growAroundQuad(regionElementIndex(i), *regionElementIndices());
	}
	
	if(numRegionElements() > 0) {
	    createBuffer(numRegionElements() * 32);
	    rebuildBuffer();
	}
}

char CollisionRegion::faceColorMatches(unsigned idx) const
{
    float u, v;
    unsigned i;
    for(i = 0; i < 29; i++) {
        u = ((float)(rand()%591))/591.f;
		v = ((float)(rand()%593))/593.f;
		if(sampleColorMatches(idx, u, v)) return 1;
    }
    return 0;
}

char CollisionRegion::sampleColorMatches(unsigned idx, float u, float v) const
{
	Float3 curCol;
	m_regionDistribution->sample(idx, u, v, (float *)&curCol);
    Vector3F difCol(curCol.x - m_sampleColor.x, curCol.y - m_sampleColor.y, curCol.z - m_sampleColor.z);
    return difCol.length() < .2f;    
}

void CollisionRegion::rebuildBuffer() 
{
    unsigned i, k;
    for(i = 0; i < numRegionElements(); i++) {
        for(k = 0; k < 4; k++)
            fillPatchEdge(regionElementIndex(i), k, i * 32 + k * 8);
    }
}

void CollisionRegion::fillPatchEdge(unsigned iface, unsigned iedge, unsigned vstart)
{
    Vector3F p;
    float u, v;
    unsigned i, acc = 0;
    for(i = 0; i < 4; i++) {
        if(iedge == 0) {
            v = 0.f;
            u = .25f * i;
        }
        else if(iedge == 1) {
            u = 1.f;
            v = .25f * i;
        }
        else if(iedge == 2) {
            v = 1.f;
            u = 1.f - .25f * i;
        }
        else {
            u = 0.f;
            v = 1.f - .25f * i;
        }
        
        m_body->pointOnPatch(iface, u, v, p);
        vertices()[acc + vstart] = p;
        acc++;
        
        if(iedge == 0) {
            u = .25f * i + .25;
        }
        else if(iedge == 1) {
            v = .25f * i + .25;
        }
        else if(iedge == 2) {
            u = 1.f - .25f * i - .25;
        }
        else {
            v = 1.f - .25f * i - .25;
        }
        
        m_body->pointOnPatch(iface, u, v, p);
        vertices()[acc + vstart] = p;
        acc++;
    }
}

const Float3 & CollisionRegion::sampleColor() const
{
    return m_sampleColor;
}

void CollisionRegion::resetActiveRegion()
{
	clearActiveRegion();
	for(unsigned i = 0; i < numRegionElements(); i++)
		addActiveRegionFace(regionElementIndex(i));
}

Vector2F CollisionRegion::curvatureAt(const Matrix33F & m0, Matrix33F & m1, const Vector3F & pos, float creep)
{
	Vector3F pop = getClosestPoint(pos);
	Vector3F n = m0.transform(Vector3F::XAxis); n.normalize();
	Vector3F t = m0.transform(Vector3F::ZAxis); t.normalize();
	Vector3F cls = getClosestPoint(pop + t * creep);
	
	Vector3F bn, n1;

	Vector3F t1 = cls - pop; t1.normalize();
	
	bn = t1.cross(n); bn.normalize();
	n1 = bn.cross(t1); n1.normalize();
	
	Matrix33F inv = m0; inv.inverse();
	t1 = inv.transform(t1);
	t1.normalize();
	t1.y = 0.f;
	
	float ry = acos(t1.dot(Vector3F::ZAxis));
	if(t1.x < 0.f) ry = -ry;
	
	Matrix33F my;
	my.rotateY(ry);
	my.multiply(m0);
	
	m1 = my; return Vector2F(ry, 0.f);
	
	inv = my; inv.inverse();
	
	n1 = inv.transform(n1);
	n1.normalize();
	n1.z = 0.f;
	float rz = acos(n1.dot(Vector3F::XAxis));
	if(n1.y < 0.f) rz = -rz;
	
	Matrix33F mz;
	mz.rotateZ(rz);
	mz.multiply(my);
	m1 = mz;
	
	return Vector2F(ry, rz);
}

float CollisionRegion::curvatureAlong(const Matrix33F & m0, const Vector3F & pos, float * lengths, unsigned n, float * angles)
{
	Matrix33F m2, m1 = m0;
	Vector3F d, p = pos;
	Vector2F cvt;
	float b = 0.f;
	for(unsigned i = 0; i < n; i++) {
		cvt = curvatureAt(m1, m2, p, lengths[i]);
		m1 = m2;
		d = Vector3F::ZAxis;
		d = m2.transform(d);      
		p += d * lengths[i];
		b += cvt.x; 
		angles[i] = cvt.x;
	} 
	return b;
}

void CollisionRegion::regionElementVertices(std::vector<unsigned> & dst) const
{
	unsigned * quad = m_body->quadIndices();
	unsigned q, k;
	for(unsigned i=0; i < numRegionElements(); i++) {
		q = regionElementIndex(i) * 4;
		for(unsigned j = 0; j < 4; j++) {
			k = quad[q + j];
			if(!IsElementIn(k, dst))
				dst.push_back(k);
		}
	}
}

void CollisionRegion::useRegionElementVertexFloat(const std::string & name)
{
	m_perVertexFloat = m_body->perVertexFloat(name);
}

void CollisionRegion::useRegionElementVertexVector(const std::string & name)
{
	m_perVertexVector = m_body->perVertexVector(name);
}

void CollisionRegion::interpolateVertexVector(Vector3F * dst)
{
    interpolateVertexVector(m_ctx->m_componentIdx, m_ctx->m_patchUV.x, m_ctx->m_patchUV.y, dst);
}

void CollisionRegion::interpolateVertexVector(unsigned faceIdx, float u, float v, Vector3F * dst)
{
	m_body->interpolateVectorOnPatch(faceIdx, u, v, m_perVertexVector, dst);
}

}