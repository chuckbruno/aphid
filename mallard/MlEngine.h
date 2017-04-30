/*
 *  MlEngine.h
 *  mallard
 *
 *  Created by jian zhang on 12/31/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include "AnorldFunc.h"
#include <AllLight.h>
#include <boost/thread.hpp>
class AdaptableStripeBuffer;
class BarbWorks;
class MlEngine : public AnorldFunc {
public:
	MlEngine();
	MlEngine(BarbWorks * w);
	virtual ~MlEngine();
	void setWorks(BarbWorks * w);
	virtual void preRender();
	virtual void render();
	virtual void postRender();
	void interruptRender();
	std::string rendererName();
protected:

private:
    void fineOutput();
	void testOutput();
	void monitorProgressing(BarbWorks * work);
	void translateCurves();
	void translateBlock(AdaptableStripeBuffer * src);
	void translateLights();
	void translateLight(BaseLight * l);
#ifdef WIN32
	AtNode * translateDistantLight(DistantLight * l);
	AtNode * translatePointLight(PointLight * l);
	AtNode * translateSquareLight(SquareLight * l);
#endif
private:
	boost::thread m_workingThread;
	boost::thread m_progressingThread;
	BarbWorks * m_barb;
};