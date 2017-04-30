#ifndef BCCLATTICE_H
#define BCCLATTICE_H

#include <CartesianGrid.h>
class GeoDrawer;
struct BezierSpline;
class BezierCurve;
class KdIntersection;
class BccLattice : public CartesianGrid
{
public:
    struct Tetrahedron {
        unsigned v[4];
    };
    
    BccLattice(const BoundingBox & bound);
    virtual ~BccLattice();
    
    void add38Node(const Vector3F & center, float h);
    void prepareTetrahedron();
    void touchIntersectedTetrahedron(const Vector3F & center, float h, 
										KdIntersection * tree);
    void untouchGreenEdges();
    void add24Tetrahedron(const Vector3F & center, float h);
    void addNeighborTetrahedron(const Vector3F & center, float h);
    void countVisitedNodes();
	void addAnchors(unsigned * anchored, KdIntersection * tree);
    void draw(GeoDrawer * drawer, unsigned * anchored);
	
	const unsigned numGreenEdges() const;
	const unsigned numTetrahedrons() const;
	const unsigned numVertices() const;
	
	void extractTetrahedronMeshData(Vector3F * points, unsigned * indices);
	void logTetrahedronMesh();
protected:

private:
    const Vector3F nodeCenter(unsigned code) const;
	void drawGreenEdges();
	void drawTetrahedrons();
	void drawTetrahedrons(unsigned * anchored);
	void drawAllNodes(GeoDrawer * drawer);
	void drawVisitedNodes(GeoDrawer * drawer);
	void encodeOctahedronVertices(const Vector3F & q, float h, int offset, unsigned * v) const;
	void touch4Tetrahedrons(unsigned * vOctahedron, KdIntersection * tree);
	void addTetrahedronsAllNodeVisited(unsigned * vOctahedron);
	bool isCurveClosetToTetrahedron(const Vector3F * p, BezierCurve * curve) const;
	bool intersectTetrahedron(const Vector3F * tet, BezierSpline * splines, unsigned numSplines) const;
	void extractPoints(Vector3F * dst);
	void extractIndices(unsigned * dst);
private:
    sdb::EdgeHash * m_greenEdges;
    Tetrahedron * m_tetrahedrons;
    unsigned m_numTetrahedrons;
    unsigned m_visitedNodes;
    static Vector3F NodeCenterOffset;
};
#endif        //  #ifndef BCCLATTICE_H

