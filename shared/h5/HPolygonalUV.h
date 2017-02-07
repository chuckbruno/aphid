#include <h5/HBase.h>

namespace aphid {

class APolygonalUV;

class HPolygonalUV : public HBase {
public:
	HPolygonalUV(const std::string & path);
	virtual ~HPolygonalUV();
	
	virtual char verifyType();
	virtual char save(APolygonalUV * poly);
	virtual char load(APolygonalUV * poly);
	
protected:
	
private:
	
};

}
