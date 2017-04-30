/*
 *  CollisionRegion.h
 *  mallard
 *
 *  Created by jian zhang on 9/21/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include <AllMath.h>
#include <Patch.h>
#include <ActiveRegion.h>
#include <deque>
namespace aphid {

class IntersectionContext;
class MeshTopology;
class AccPatchMesh;
class PatchTexture;
class BaseSphere;
class CollisionRegion : public ActiveRegion {
public:
	CollisionRegion();
	virtual ~CollisionRegion();
	void clearCollisionRegion();
	
	AccPatchMesh * bodyMesh() const;
	MeshTopology * topology() const;
	
	Vector3F getIntersectPoint(const Vector3F & origin, const Vector3F & dir);
	Vector3F getClosestPoint(const Vector3F & origin);
	Vector3F getClosestNormal(const Vector3F & origin, float maxD, Vector3F & pos);
	
	virtual void setBodyMesh(AccPatchMesh * mesh);
	virtual void resetCollisionRegion(const std::deque<unsigned> & src);
	virtual void resetCollisionRegion(unsigned idx);
	virtual void resetCollisionRegionByDistance(unsigned idx, const Vector3F & center, float maxD);
	virtual void resetCollisionRegionAround(unsigned idx, const BoundingBox & bbox);
	virtual void resetCollisionRegionAround(unsigned idx, const float & vicinity);
	virtual void resetCollisionRegionAround(unsigned idx, const BaseSphere & sph);

	virtual void closestPoint(const Vector3F & origin, IntersectionContext * ctx) const;
	virtual void pushPlane(Patch::PushPlaneContext * ctx) const;
	
	unsigned numRegionElements() const;
	unsigned regionElementIndex(unsigned idx) const;
	
	unsigned regionElementStart() const;
	void setRegionElementStart(unsigned x);
	
	void setDistributionMap(PatchTexture * image);
	void selectRegion(unsigned idx, const Vector2F & patchUV);
	
	std::vector<unsigned> * regionElementIndices();
	
	char faceColorMatches(unsigned idx) const;
	
	virtual void rebuildBuffer();
	virtual void resetActiveRegion();
	
	void neighborFaces(unsigned idx, std::vector<unsigned> & dst);
	char sampleColorMatches(unsigned idx, float u, float v) const;
	const Float3 & sampleColor() const;
	
	Vector2F curvatureAt(const Matrix33F & m0, Matrix33F & m1, const Vector3F & pos, float creep);
	float curvatureAlong(const Matrix33F & m0, const Vector3F & pos, float * lengths, unsigned n, float * angles);

	void regionElementVertices(std::vector<unsigned> & dst) const;
	void useRegionElementVertexFloat(const std::string & name);
	void useRegionElementVertexVector(const std::string & name);
	void interpolateVertexVector(Vector3F * dst); 
	void interpolateVertexVector(unsigned faceIdx, float u, float v, Vector3F * dst);
private:
    void fillPatchEdge(unsigned iface, unsigned iedge, unsigned vstart);
	MeshTopology * m_topo;
	AccPatchMesh * m_body;
	std::vector<unsigned> m_regionElementIndices;
	unsigned m_regionElementStart;
	int m_hitElement;
	PatchTexture * m_regionDistribution;
	IntersectionContext * m_ctx;
	Float3 m_sampleColor;
	float * m_perVertexFloat;
	Vector3F * m_perVertexVector;
};

}