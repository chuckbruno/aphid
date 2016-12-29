/*
 *  DrawBox.h
 *  
 *
 *  Created by jian zhang on 2/4/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

namespace aphid {

class BoundingBox;
class Vector3F;
class DrawBox {

public:

	DrawBox();
	virtual ~DrawBox();
	
protected:

	void drawWireBox(const float * center, const float & scale) const;
	void drawSolidBox(const float * center, const float & scale) const;
	void drawWireBox(const float * center, const float * scale) const;
	void drawSolidBox(const float * center, const float * scale) const;
	void drawBoundingBox(const BoundingBox * box) const;
	void drawWiredBoundingBox(const BoundingBox * box) const;
	void drawSolidBoundingBox(const BoundingBox * box) const;
	void drawSolidBoxArray(const float * data,
						const unsigned & count,
						const unsigned & stride = 1) const;
	void drawSolidBoxArray(const float * ps,
						const float * ns,
						const unsigned & count) const;
	void drawWireBoxArray(const float * data,
						const unsigned & count,
						const unsigned & stride = 1) const;
	void setSolidBoxDrawBuffer(const float * center, const float & scale,
						Vector3F * position, Vector3F * normal) const;
						
	void drawHLWireBox(const float * v) const;
	void drawHLSolidBox(const float * v) const;
	void drawHLBox(const float * v) const;
	void drawWiredTriangleArray(const float * ps,
						const unsigned & count,
						const float * col) const;
	void drawSolidTriangleArray(const float * ps,
						const float * ns,
						const unsigned & count,
						const float * col) const;
private:
	static const float UnitBoxLine[24][3];
	static const float UnitBoxTriangle[36][3];
	static const float UnitBoxNormal[36][3];
	static const int HLBoxLine[24][3];
	static const int HLBoxTriangle[36][3];
};

}