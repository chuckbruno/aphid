#ifndef HPOLYGONALMESH_H
#define HPOLYGONALMESH_H

#include <h5/HBase.h>

namespace aphid {

class APolygonalMesh;

class HPolygonalMesh : public HBase {
public:
	HPolygonalMesh(const std::string & path);
	virtual ~HPolygonalMesh();
	
	virtual char verifyType();
	virtual char save(APolygonalMesh * poly);
	virtual char load(APolygonalMesh * poly);
	
protected:
	
private:
	
};

}
#endif        //  #ifndef HPOLYGONALMESH_H

