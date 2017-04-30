/*
 *  BaseLight.h
 *  aphid
 *
 *  Created by jian zhang on 1/10/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include "BaseTransform.h"

namespace aphid {

class BaseLight : public BaseTransform {
public:
	BaseLight();
	virtual ~BaseLight();
	
	void setLightColor(float r, float g, float b);
	void setLightColor(const Float3 c);
	Float3 lightColor() const;
	
	void setIntensity(float x);
	float intensity() const;
	
	void setSamples(int x);
	int samples() const;
	
	void setCastShadow(bool x);
	bool castShadow() const;
protected:

private:
	Float3 m_lightColor;
	float m_intensity;
	int m_samples;
	bool m_castShadow;
};

}