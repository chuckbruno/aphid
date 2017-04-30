#ifndef MASSSYSTEM_H
#define MASSSYSTEM_H

/*
 *  MassSystem.h
 *  testcudafem
 *
 *  Created by jian zhang on 6/10/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */
#include <DynamicBody.h>
#include <Vector3F.h>
class BaseBuffer;
class MassSystem : public aphid::DynamicBody {
public:
	MassSystem();
	virtual ~MassSystem();
	
	virtual void create(unsigned maxNumTetrahedrons, unsigned maxNumTriangles, unsigned maxNumPoints);
	const unsigned numPoints() const;
	const unsigned numTetrahedrons() const;
	const unsigned numTriangles() const;
	const unsigned numTriangleFaceVertices() const;
	
	float * hostX();
	float * hostXi();
	float * hostV();
	float * hostMass();
	unsigned * hostAnchor();
	unsigned * hostTriangleIndices();
    unsigned * hostTetrahedronIndices();
	
	virtual const int elementRank() const;
	virtual const unsigned numElements() const;

    void resetVelocity();
    const float totalMass() const;
	
	virtual void integrate(float dt);
	virtual float energy();
    
    void setHostX(float * src);
    
protected:
	void setNumPoints(unsigned x);
	void setNumTetrahedrons(unsigned x);
	void setNumTriangles(unsigned x);
	
	void addPoint(float * src);
	void addTetrahedron(unsigned a, unsigned b, unsigned c, unsigned d);
	void addTriangle(unsigned a, unsigned b, unsigned c);
	void setTotalMass(float x);
    void setInitialTotalMass(float x);
    float initialTotalMass() const;
    
    void setAnchoredValue(unsigned i, unsigned anchorInd);
	bool isAnchoredPoint(unsigned i);
	float averageNodeMass() const;
	
private:
	BaseBuffer * m_hostX;
	BaseBuffer * m_hostXi;
	BaseBuffer * m_hostV;
	BaseBuffer * m_hostMass;
	BaseBuffer * m_hostAnchor;
    BaseBuffer * m_hostTetrahedronIndices;
	BaseBuffer * m_hostTriangleIndices;
    unsigned m_numPoints, m_numTetrahedrons, m_numTriangles;
	unsigned m_maxNumPoints, m_maxNumTetrahedrons, m_maxNumTriangles;
    float m_totalMass, m_initialTotalMass;
};
#endif        //  #ifndef MASSSYSTEM_H
