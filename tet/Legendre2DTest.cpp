/*
 *  Legendre2DTest.cpp
 *  foo
 *  
 *  Created by jian zhang on 7/14/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#include "Legendre2DTest.h"
#include <GridTables.h>
#include <Calculus.h>
#include <ANoise3.h>

using namespace aphid;
namespace ttg {

Legendre2DTest::Legendre2DTest() 
{}

Legendre2DTest::~Legendre2DTest() 
{}
	
const char * Legendre2DTest::titleStr() const
{ return "2D Legendre Polynomial Approximation"; }

bool Legendre2DTest::init()
{
	int i,j, k;
	
#if 0
	float ppv[257*7];
	calc::LegendrePolynomial::sample(257, 6, ppv, -1.f, 1.f);
	for(i=0;i<257*7;++i)
		std::cout<<" "<<ppv[i]<<",";
#endif

#if 0
	for(i=0;i<7;++i)
		std::cout<<" "<< ( 2.f / ( float ) ( 2 * i + 1 ) )<<", ";
#endif

#define N_H_SEG 16
#define N_SEG 32
	const float du = 1.f / N_H_SEG;
	Vector3F smp;
	m_exact.create(32, 32);
	m_appro.create(32, 32);
	
	for(j=0;j<=N_SEG;++j) {
		for(i=0;i<=N_SEG;++i) {
			smp.set(du*i - 1.f, 0.f, du*j - 1.f);
			smp.y = exactMeasure(smp.x, smp.z);
			*m_exact.quadP(i, j) = smp;
		}
	}

	calc::gaussQuadratureRule(N_ORD, m_Wi, m_Xi);
	std::cout<<"\n gauss quadrate rule of order "<<N_ORD;
	calc::printValues<float>("wi", N_ORD, m_Wi);
	calc::printValues<float>("xi", N_ORD, m_Xi);
	calc::legendreRule(N_ORD, N_P, m_Pv, m_Xi);
	calc::printValues<float>("poly", N_ORD * (N_P+1), m_Pv);
	
	int indx[N_DIM];
	
	int li;
	int rnk = 0;
	int neval = 0;
	for(;;) {
		calc::tuple_next(1, N_ORD, N_DIM, &rnk, indx);
		
		if(rnk==0)
			break;
	
		calc::printValues<int>("tuple space", N_DIM, indx);
		std::cout<<"\n measure at ("<<m_Xi[indx[0]-1]<<","<<m_Xi[indx[1]-1]<<")";
		li = calc::lexIndex(N_DIM, N_ORD, indx, -1);
		m_Yij[li] = exactMeasure(m_Xi[indx[0]-1], m_Xi[indx[1]-1] );
		
		neval++;
	}
	
	for(j=0;j<=N_P;++j) {
		indx[1] = j;
		for(i=0;i<=N_P;++i) {
			indx[0] = i;
			k = calc::lexIndex(N_DIM, N_P+1, indx);
			m_Coeij[k] = computeCoeff(i, j);
		}
	}
	
	for(j=0;j<=N_SEG;++j) {
		for(i=0;i<=N_SEG;++i) {
			smp.set(du*i - 1.f, 0.f, du*j - 1.f);
			smp.y = approximate(smp.x, smp.z);
			*m_appro.quadP(i, j) = smp;
		}
	}
	
	std::cout<<"\n done!";
	std::cout.flush();
	return true;
}

float Legendre2DTest::computeCoeff(int l, int m) const
{
	int indx[N_DIM];
	float fpp[N_ORD2];
	int i,j,k;
	
	for(j=0;j<N_ORD;++j) {
		indx[1] = j;
		for(i=0;i<N_ORD;++i) {
			indx[0] = i;
				
			k = calc::lexIndex(N_DIM, N_ORD, indx);
			
/// f(x,y)P(l,x)P(m,y)
			fpp[k] = m_Yij[k] * m_Pv[i+N_ORD*l] * m_Pv[j+N_ORD*m];
		}
	}
	
	float result = calc::gaussQuadratureRuleIntegrate(N_DIM, N_ORD,
												m_Xi, m_Wi, fpp);
	result /= calc::LegendrePolynomial::norm2(l) * calc::LegendrePolynomial::norm2(m); 
	std::cout<<"\n C("<<l<<","<<m<<") "<<result;
	return result;
}

float Legendre2DTest::approximate(const float & x, const float & y) const
{
#define U_P 3
	float result = 0.f;
	int indx[N_DIM];
	int i, j, k;
	for(j=0;j<=U_P;++j) {
		indx[1] = j;
		for(i=0;i<=U_P;++i) {
			indx[0] = i;
			
			k = calc::lexIndex(N_DIM, N_P+1, indx);
			result += m_Coeij[k] 
						* calc::LegendrePolynomial::P(i, x)
						* calc::LegendrePolynomial::P(j, y);
		
		}
	}
	return result;
}

float Legendre2DTest::exactMeasure(const float & x, const float & y) const
{
	const Vector3F at(x, 0.f, y);
	const Vector3F orp(.6241f, .8534f, .2786f);
	return ANoise3::Fbm((const float *)&at,
										(const float *)&orp,
										.8854f,
										5,
										1.24f,
										.395f);
}

void Legendre2DTest::draw(GeoDrawer * dr)
{
	glColor3f(0.f, 0.f, 0.f);
	glPushMatrix();
	glScalef(10.f, 10.f, 10.f);
	dr->m_wireProfile.apply();
	dr->geometry(&m_exact);
	
	glColor3f(0.13f, 1.f, 0.43f);
	dr->geometry(&m_appro);
	
	glColor3f(1.f, 0.33f, 0.33f);
	
	int indx[N_DIM];
	int i,j;
	for(j=0;j<N_ORD;++j) {
		indx[1] = j;
		for(i=0;i<N_ORD;++i) {
			indx[0] = i;
			dr->arrow(Vector3F(m_Xi[i], 0.f, m_Xi[j]), 
						Vector3F(m_Xi[i], m_Yij[calc::lexIndex(N_DIM, N_ORD, indx)], m_Xi[j]));
		}
	}
	
	glPopMatrix();
}

}