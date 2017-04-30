/*
 *  fltbanks.h
 *  
 *
 *  Created by jian zhang on 9/14/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef WLA_FLT_BANKS_H
#define WLA_FLT_BANKS_H
#include <math/ATypes.h>

namespace aphid {

namespace wla {

class Dtf {

public:
/// get the filters
/// af[4][10] analysis filters
/// sf[4][10] synthesis filters
	static void fsfarras(float ** af, float ** sf);
	static void dualflt(float ** af, float ** sf);

	static const float FirstStageUpFarrasAnalysis[2][10];
	static const float FirstStageUpFarrasSynthesis[2][10];
	static const float FirstStageDownFarrasAnalysis[2][10];
	static const float FirstStageDownFarrasSynthesis[2][10];
	static const float UpAnalysis[2][10];
	static const float DownAnalysis[2][10];
	static const float UpSynthesis[2][10];
	static const float DownSynthesis[2][10];
	static const float FarrasAnalysisFilter[2][10];
	static const float FarrasSynthesisFilter[2][10];
	
};

/// http://cn.mathworks.com/help/signal/ref/upsample.html
/// increase sampling rate by integer factor p with phase offset
/// by inserting p – 1 zeros between samples
float * upsample(int & ny, const float * x, const int & n, 
				const int & p, const int & phase = 0);
				
/// http://cn.mathworks.com/help/signal/ref/downsample.html
/// decrease sampling rate by integer factor p with phase offset
float * downsample(int & ny, const float * x, const int & n, 
				const int & p, const int & phase = 0);

int periodic(const int & i, const int & n);

/// delay (p>0) or haste (p<0) the signal
float * circshift(const float * x, const int & n, const int & p);

/// apply finite impulse response filter 
/// to signal X[N]
/// W[M] response coefficients
float * fir(const float * x, const int & n,
			const float * w, const int & m);

/// http://learn.mikroe.com/ebooks/digitalfilterdesign/chapter/examples/
/// Hann window low pass filter coefficients 11-tap
float * hannLowPass(int & n);
float * hannHighPass(int & n);

float * firlHann(const float * x, const int & n);
float * firhHann(const float * x, const int & n);

/// analysis and synthesis filters of order 10
/// af[2][10] analysis filter
/// sf[2][10] synthesis filter
/// first column is low pass, second is high pass
void farras(float ** af, float ** sf);

/// http://eeweb.poly.edu/iselesni/WaveletSoftware/allcode/afb.m
/// analysis filter bank
/// apply farras filters and downsample
/// X[N] input signal
/// lo low frequency output
/// hi high frequency output
/// M output length
void afb(const float * x, const int & n,
		float * & lo, float * & hi, int & m);

/// http://eeweb.poly.edu/iselesni/WaveletSoftware/allcode/sfb.m
/// synthesisi filter bank
/// upsample input, apply farras filters, add up
/// lo[M] low frequency input 
/// hi[M] high frequency input
/// M input length
/// Y output signal
/// N output length
void sfb(const float * lo, const float * hi, const int & m,
		float * & y, int & n);

/// dual tree
/// analysis
void afbflt(const float * x, const int & n,
		VectorN<float> & lo, VectorN<float> & hi,
		const float flt[2][10]);
/// synthesis
void sfbflt(VectorN<float> & y,
		const VectorN<float> & lo, const VectorN<float> & hi,
		const float flt[2][10]);
		
}

}

#endif
