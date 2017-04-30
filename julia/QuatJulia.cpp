/*
 *  QuatJulia.cpp
 *  
 *
 *  Created by jian zhang on 1/1/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 *  http://paulbourke.net/fractals/quatjulia/
 *  http://www.chaospro.de/documentation/html/fractaltypes/quaternions/theory.htm
 */

#include "QuatJulia.h"
#include <HWorld.h>

namespace jul {

Float4 quatProd(const Float4 & a, const Float4 & b)
{
	return Float4(
			a.x * b.x - a.y * b.y - a.z * b.z - a.w * b.w,
			a.x * b.y + a.y * b.x + a.z * b.w - a.w * b.z,
			a.x * b.z - a.y * b.w + a.z * b.x + a.w * b.y,
			a.x * b.w + a.y * b.z - a.z * b.y + a.w * b.x);
}

Float4 quatCongujate(const Float4 & a)
{
	return Float4(a.x, -a.y, -a.z, -a.w);
}

Float4 quatSq(const Float4 & a)
{
	return Float4(
			a.x * a.x - a.y * a.y - a.z * a.z - a.w * a.w,
			2.0 * a.x * a.y,
			2.0 * a.x * a.z,
			2.0 * a.x * a.w);
}

QuatJulia::QuatJulia(Parameter * param) 
{
	m_c = Float4(-0.02,-0.0156,-0.563,-0.4);
	m_numIter = 10;
	m_numGrid = 360;
	m_scaling = 48.f;
	
	HObject::FileIO.open(param->outFileName().c_str(), HDocument::oCreate);
	m_tree = new sdb::HWorldGrid<sdb::HInnerGrid<hdata::TFloat, 4, 256 >, cvx::Sphere >("/grid");
	m_tree->setGridSize(m_scaling / 32.f);
	generate();
	m_tree->save();
	m_tree->close();
	HWorld gworld;
	gworld.save();
	gworld.close();
	HObject::FileIO.close();
}

QuatJulia::~QuatJulia()
{ delete m_tree; }

void QuatJulia::generate()
{
/// eval at uniform grid
	int i, j, k;
	const float grid = 1.f / (float)m_numGrid;
	const Vector3F origin(-.5f+grid * .5f, 
							-.5f+grid * .5f, 
							-.5f+grid * .5f);
	int n = 0;
	for(k=0; k<m_numGrid; ++k ) {
		for(j=0; j<m_numGrid; ++j ) {
			for(i=0; i<m_numGrid; ++i ) {
				Vector3F sample = origin + Vector3F(grid * i, grid * j, grid * k);
				if( evalAt( sample*3.2f ) > 0.f ) {
					n++;
					cvx::Sphere sp;
					sample *= m_scaling;
					sp.set(sample, .001f);
					m_tree->insert((const float *)&sample, sp);
				}
			}
		}
	}
	m_tree->finishInsert();
}

float QuatJulia::evalAt(const Vector3F & at) const
{
/// Quaternion Julia Fractals
	Float4 z(at.x, at.y, at.z, 0.1f);
	Float4 z2(1.f, 0.f, 0.f, 0.f);

	float n = 0.0;
	float sqr_abs_z = 0.0;

	while (n < m_numIter)
	{
		z2 = quatProd(z, z2) * 2.f;
		z = quatSq(z) + m_c;
		sqr_abs_z = z.dot(z);
		if (sqr_abs_z >= 1.f)
			break;

		n++;
	}

	return 1.f - sqr_abs_z;
}

}