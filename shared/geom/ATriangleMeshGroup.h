/*
 *  ATriangleMeshGroup.h
 *  aphid
 *
 *  Created by jian zhang on 7/6/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#include <geom/ATriangleMesh.h>
#include <foundation/AStripedModel.h>

namespace aphid {

class ATriangleMeshGroup : public ATriangleMesh, public AStripedModel {
public:
	ATriangleMeshGroup();
	virtual ~ATriangleMeshGroup();
	
	void create(unsigned np, unsigned nt, unsigned ns);
	
private:

};

}
