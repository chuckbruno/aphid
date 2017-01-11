/*
 *  DOP8Builder.cpp
 *  
 *
 *  Created by jian zhang on 1/11/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#include "DOP8Builder.h"
#include <math/AOrientedBox.h>
#include <math/Ray.h>

namespace aphid {

DOP8Builder::DOP8Builder()
{}

void DOP8Builder::build(const AOrientedBox & ob)
{
	const Vector3F & et = ob.extent();
	const float * dopEt = ob.dopExtent();
/// top side
	int nvert = 4;
	m_vert[0].set(-et.x, -et.y, et.z);
	m_vert[1].set( et.x, -et.y, et.z);
	m_vert[2].set( et.x,  et.y, et.z);
	m_vert[3].set(-et.x,  et.y, et.z);
	m_ntri = 2;
	m_tri[0] = 0; m_tri[1] = 1; m_tri[2] = 2;
	m_tri[3] = 0; m_tri[4] = 2; m_tri[5] = 3;
/// rotate 45 deg
	Vector3F rgt(.7071067f, .7071067f, 0.f);
	Vector3F up(-.7071067f, .7071067f, 0.f);
	
/// max 8 edges
    Vector3F edgeNor[8];
	int edgei[16];
	int edgec = 4;
	edgei[0] = 0; edgei[1] = 1;
	edgei[2] = 1; edgei[3] = 2;
	edgei[4] = 2; edgei[5] = 3;
	edgei[6] = 3; edgei[7] = 0;
	edgeNor[0].set(0.f,-1.f, 0.f);
	edgeNor[1].set(1.f, 0.f, 0.f);
	edgeNor[2].set(0.f, 1.f, 0.f);
	edgeNor[3].set(-1.f, 0.f, 0.f);
	int splitEdge = 3;
	BoundingBox box(-et.x, -et.y, -et.z,
					 et.x,  et.y,  et.z);
	
	float cutThre = et.y < et.x ? et.y : et.x; 
	cutThre *= .19f;
	Vector3F o = rgt * dopEt[0] + up * dopEt[3];
	Vector3F d = rgt * dopEt[0] + up * dopEt[2];
	Vector3F phit;
	Ray incident(o, d);
	float tmin, tmax;
	if(box.intersect(incident, &tmin, &tmax) ) {
		if(tmax - tmin > cutThre) {
			phit = incident.travel(tmax);
			m_vert[0].x = phit.x;
			m_vert[0].y = phit.y;
			
			phit = incident.travel(tmin);
			m_vert[nvert].x = phit.x;
			m_vert[nvert].y = phit.y;
			m_vert[nvert].z = et.z;
			m_tri[m_ntri*3] = 0; m_tri[m_ntri*3+1] = nvert-1; m_tri[m_ntri*3+2] = nvert;
			
            //std::cout<<"\n split e"<<splitEdge<<" "
            //        <<edgei[splitEdge*2]<<" - "<<edgei[splitEdge*2+1];
            
            edgei[edgec*2] = nvert;
			edgei[edgec*2+1] = edgei[splitEdge*2+1];
			edgei[splitEdge*2+1] = nvert;
            
            //std::cout<<" into "<<edgei[splitEdge*2]<<" - "<<edgei[splitEdge*2+1]
            //    <<" and "<<edgei[edgec*2]<<" - "<<edgei[edgec*2+1];
                    
			edgeNor[edgec].set(-.7071f, -.7071f, 0.f);
			edgec++;
			nvert++;
			m_ntri++;
		}
	}
	
    splitEdge = 0;
    
	o = rgt * dopEt[0] + up * dopEt[2];
	d = rgt * dopEt[1] + up * dopEt[2];
	incident = Ray(o, d);
	if(box.intersect(incident, &tmin, &tmax) ) {
		if(tmax - tmin > cutThre) {
			phit = incident.travel(tmax);
			m_vert[1].x = phit.x;
			m_vert[1].y = phit.y;
			
			phit = incident.travel(tmin);
			m_vert[nvert].x = phit.x;
			m_vert[nvert].y = phit.y;
			m_vert[nvert].z = et.z;
			m_tri[m_ntri*3] = 0; m_tri[m_ntri*3+1] = nvert; m_tri[m_ntri*3+2] = 1;
			
            //std::cout<<"\n split e"<<splitEdge<<" "
            //        <<edgei[splitEdge*2]<<" - "<<edgei[splitEdge*2+1];
            
            edgei[edgec*2] = nvert;
			edgei[edgec*2+1] = edgei[splitEdge*2+1];
            edgei[splitEdge*2 + 1] = nvert;
			
            //std::cout<<" into "<<edgei[splitEdge*2]<<" - "<<edgei[splitEdge*2+1]
            //    <<" and "<<edgei[edgec*2]<<" - "<<edgei[edgec*2+1];
            
			edgeNor[edgec].set(.7071f, -.7071f, 0.f);
			edgec++;
			nvert++;
			m_ntri++;
		}
	}
	
    splitEdge = 1;
	o = rgt * dopEt[1] + up * dopEt[2];
	d = rgt * dopEt[1] + up * dopEt[3];
	incident = Ray(o, d);
	if(box.intersect(incident, &tmin, &tmax) ) {
		if(tmax - tmin > cutThre) {
			phit = incident.travel(tmax);
			m_vert[2].x = phit.x;
			m_vert[2].y = phit.y;
			
			phit = incident.travel(tmin);
			m_vert[nvert].x = phit.x;
			m_vert[nvert].y = phit.y;
			m_vert[nvert].z = et.z;
			m_tri[m_ntri*3] = 1; m_tri[m_ntri*3+1] = nvert; m_tri[m_ntri*3+2] = 2;
			
            //std::cout<<"\n split e"<<splitEdge<<" "
            //        <<edgei[splitEdge*2]<<" - "<<edgei[splitEdge*2+1];
            
            edgei[edgec*2] = nvert;
			edgei[edgec*2+1] = edgei[splitEdge*2+1];
            edgei[splitEdge*2 + 1] = nvert;
			
            //std::cout<<" into "<<edgei[splitEdge*2]<<" - "<<edgei[splitEdge*2+1]
            //    <<" and "<<edgei[edgec*2]<<" - "<<edgei[edgec*2+1];
            
                
			edgeNor[edgec].set(.7071f, .7071f, 0.f);
			edgec++;
			nvert++;
			m_ntri++;
		}
	}
	
    splitEdge = 2;
	o = rgt * dopEt[1] + up * dopEt[3];
	d = rgt * dopEt[0] + up * dopEt[3];
	incident = Ray(o, d);
	if(box.intersect(incident, &tmin, &tmax) ) {
		if(tmax - tmin > cutThre) {
			phit = incident.travel(tmax);
			m_vert[3].x = phit.x;
			m_vert[3].y = phit.y;
			
			phit = incident.travel(tmin);
			m_vert[nvert].x = phit.x;
			m_vert[nvert].y = phit.y;
			m_vert[nvert].z = et.z;
			m_tri[m_ntri*3] = 2; m_tri[m_ntri*3+1] = nvert; m_tri[m_ntri*3+2] = 3;
			
            //std::cout<<"\n split e"<<splitEdge<<" "
            //        <<edgei[splitEdge*2]<<" - "<<edgei[splitEdge*2+1];
            
            edgei[edgec*2] = nvert;
			edgei[edgec*2+1] = edgei[splitEdge*2+1];
            edgei[splitEdge*2 + 1] = nvert;
			
            //std::cout<<" into "<<edgei[splitEdge*2]<<" - "<<edgei[splitEdge*2+1]
             //   <<" and "<<edgei[edgec*2]<<" - "<<edgei[edgec*2+1];
            
			edgeNor[edgec].set(-.7071f, .7071f, 0.f);
			edgec++;
			nvert++;
			m_ntri++;
		}
	}

/// copy to bottom side
	for(int i=0; i<nvert; ++i) {
		m_vert[i+nvert] = m_vert[i];
		m_vert[i+nvert].z = -et.z;
	}
	
	const Matrix33F & space = ob.orientation();
	const Vector3F & cnt = ob.center();
	nvert += nvert;
	for(int i=0; i<nvert; ++i) {
		m_vert[i] = space.transform(m_vert[i]) + cnt;
	}
	
	Vector3F topNor(0.f, 0.f, 1.f);
    topNor = space.transform(topNor);
	Vector3F bottomNor(0.f, 0.f, -1.f);
	bottomNor = space.transform(bottomNor);
    
	for(int i=0; i<edgec; ++i) {
		edgeNor[i] = space.transform(edgeNor[i]);
	}
	
    //for(int i=0; i<edgec; ++i) {
    //    std::cout<<"\n e"<<i<<" "<<edgei[i*2]<<" - "<<edgei[i*2+1];
    //}
	
/// nor to face nor
	for(int i=0; i<m_ntri*3; ++i) {
        m_facenor[i] = topNor;
	}
	for(int i=m_ntri*3; i<m_ntri*6; ++i) {
		m_facenor[i] = bottomNor;
	}
	int lateralOffset = m_ntri * 6;
	for(int i=0; i<edgec; ++i) {
		const Vector3F lateralNor = edgeNor[i];
		m_facenor[lateralOffset + i*6] = lateralNor;
		m_facenor[lateralOffset + i*6+1] = lateralNor;
		m_facenor[lateralOffset + i*6+2] = lateralNor;
		m_facenor[lateralOffset + i*6+3] = lateralNor;
		m_facenor[lateralOffset + i*6+4] = lateralNor;
		m_facenor[lateralOffset + i*6+5] = lateralNor;
	}

/// bottom side
    for(int i=0; i<m_ntri; ++i) {
		m_tri[(i+ m_ntri)*3] = m_tri[i*3] + edgec;
		m_tri[(i+ m_ntri)*3+1] = m_tri[i*3+2] + edgec;
		m_tri[(i+ m_ntri)*3+2] = m_tri[i*3+1] + edgec;
	}
	m_ntri += m_ntri;
    
/// connect top and bottom
	for(int i=0; i<edgec; ++i) {
		m_tri[m_ntri*3] = edgei[i*2];
		m_tri[m_ntri*3+1] = edgei[i*2] + edgec;
		m_tri[m_ntri*3+2] = edgei[i*2+1];
		m_ntri++;
		m_tri[m_ntri*3] = edgei[i*2] + edgec;
		m_tri[m_ntri*3+1] = edgei[i*2+1] + edgec;
		m_tri[m_ntri*3+2] = edgei[i*2+1];
		m_ntri++;
	}
	
/// vert to face vert
	for(int i=0; i<m_ntri*3; ++i) {
		m_facevert[i] = m_vert[m_tri[i]];
	}
    
    //for(int i=0; i<m_ntri*3; ++i) {
    //   std::cout<<"\n nor "<<i<<m_facenor[i];
	//}
}

const int & DOP8Builder::numTriangles() const
{ return m_ntri; }

const Vector3F * DOP8Builder::vertex() const
{ return m_facevert; }

const Vector3F * DOP8Builder::normal() const
{ return m_facenor; }

const int *  DOP8Builder::triangleIndices() const
{ return m_tri; }

}
