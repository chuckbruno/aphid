/*
 *  BccTetrahedralize.h
 *  
 *
 *  Created by jian zhang on 6/14/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef TTG_BCC_TETRAHEDRALIZE_H
#define TTG_BCC_TETRAHEDRALIZE_H
#include "SuperformulaPoisson.h"
#include "TetrahedralMesher.h"

namespace ttg {

class BccTetrahedralize : public SuperformulaPoisson {

	TetrahedralMesher m_mesher;
	int m_sampleBegin;
	float m_pntSz;
	std::vector<int> m_itetnegative;
	
public:
	BccTetrahedralize();
	virtual ~BccTetrahedralize();
	
	virtual const char * titleStr() const;
	virtual bool createSamples();
	virtual void draw(aphid::GeoDrawer * dr);
	
protected:
	
private:
	void drawFrontEdges();
	void drawFrontTets(aphid::GeoDrawer * dr);
	void drawRedBlueGreen(aphid::GeoDrawer * dr);
	void drawTetnegative(aphid::GeoDrawer * dr);
	
};

}
#endif