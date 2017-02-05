#include <QtGui>
#include <QtOpenGL>
#include <BaseCamera.h>
#include <PerspectiveView.h>
#include "widget.h"
#include <GeoDrawer.h>
#include <ogl/DrawGrid.h>
#include <ttg/RedBlueRefine.h>
#include <ttg/TetrahedronDistanceField.h>
#include <ogl/DrawGraph.h>
#include <geom/PrimInd.h>

using namespace aphid;

GLWidget::GLWidget(QWidget *parent) : Base3DView(parent)
{
	perspCamera()->setFarClipPlane(20000.f);
	perspCamera()->setNearClipPlane(1.f);
	orthoCamera()->setFarClipPlane(20000.f);
	orthoCamera()->setNearClipPlane(1.f);
	usePerspCamera();
	
}

GLWidget::~GLWidget()
{}

static const float stetvs[4][3] = {
{ -13.f, -3.f, -9.f}, 
{ 0.f, 17.f, -5.f},
{ 16.f, 2.f, -10.f}, 
{ 4.f, -12.f, 12.f}
};

void GLWidget::clientInit()
{
	cvx::Tetrahedron tetra;
	tetra.set(Vector3F(stetvs[0]), 
				Vector3F(stetvs[1]),
				Vector3F(stetvs[2]), 
				Vector3F(stetvs[3]) );
		
    TetrahedronGridUtil<5 > tu4;
	m_grd = new MesherT::GridT(tetra, 0);
    
#if 0
    TFTNode anode;
    const int nn = m_grd->numPoints();
    std::cout << "\n n node " << nn;
    for(int i=0;i<nn;++i) {
        anode._distance = m_grd->pos(i).y  + 1.4f * sin(m_grd->pos(i).x * .2f + 1.f)
                                        - cos(m_grd->pos(i).z  * .3f - 2.f) ;
        m_grd->setValue(anode, i);
    }
#endif
    
    m_mesher.setGrid(m_grd);
    
    m_fieldDrawer = new FieldDrawerT;
    m_fieldDrawer->initGlsl();
    
static const float scCorners[6][3] = {
{4, -2, 0},
{2, 17, -9},
{-2, 0, -1},
{9, 6, -13},
{6, 12, -11},
{4, -5, 1}
};

    cvx::Triangle * ta = new cvx::Triangle;
	ta->set(Vector3F(scCorners[2]), Vector3F(scCorners[0]), Vector3F(scCorners[1]) );
	m_ground.push_back(ta);
	cvx::Triangle * tb = new cvx::Triangle;
	tb->set(Vector3F(scCorners[1]), Vector3F(scCorners[0]), Vector3F(scCorners[4]) );
	m_ground.push_back(tb);
    cvx::Triangle * tc = new cvx::Triangle;
	tc->set(Vector3F(scCorners[4]), Vector3F(scCorners[0]), Vector3F(scCorners[3]) );
	m_ground.push_back(tc);
    cvx::Triangle * td = new cvx::Triangle;
	td->set(Vector3F(scCorners[2]), Vector3F(scCorners[1]), Vector3F(-10, 5, -13) );
	m_ground.push_back(td);
	cvx::Triangle * te = new cvx::Triangle;
	te->set(Vector3F(scCorners[3]), Vector3F(scCorners[0]), Vector3F(scCorners[5]) );
	m_ground.push_back(te);
    cvx::Triangle * tf = new cvx::Triangle;
	tf->set(Vector3F(scCorners[5]), Vector3F(scCorners[0]), Vector3F(scCorners[2]) );
	m_ground.push_back(tf);
    
	m_sels.insert(0);
	m_sels.insert(1);
    m_sels.insert(2);
    m_sels.insert(3);
    m_sels.insert(4);
    m_sels.insert(5);
    
    TIntersect fintersect(&m_sels, &m_ground);
    
    Vector3F agp, agn;
    fintersect.getAggregatedPositionNormal(agp, agn);
        
    m_mesher.field()->calculateDistance<TIntersect>(m_grd, &fintersect, agp, agn, 1.f);
    //m_field->updateGrid(m_grd);
    
    m_mesher.triangulate();
    
}

void GLWidget::clientDraw()
{
	//updatePerspectiveView();
	//getDrawer()->frustum(perspectiveView()->frustum() );
	
	getDrawer()->m_markerProfile.apply();
	getDrawer()->setColor(.5f, .125f, .5f);
	drawGround();
    
    getDrawer()->setColor(.025f, .25f, .125f);
	//drawSolidGrid();
    drawField();
    drawCellCut();
    drawTriangulation();
/*
    glEnable(GL_CULL_FACE);
    glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
    
    cvx::Tetrahedron tetra;
	tetra.set(Vector3F(stetvs[0]), 
				Vector3F(stetvs[1]),
				Vector3F(stetvs[2]), 
				Vector3F(stetvs[3]) );
    drawASolidTetrahedron(tetra);            
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
*/
}

void GLWidget::drawCellCut()
{
    TIntersect fintersect(&m_sels, &m_ground);
    
    glBegin(GL_LINES);
    cvx::Tetrahedron atet;
    const int n = m_grd->numCells();
    for(int i=0;i<n;++i) {
        m_grd->getCell(atet, i);
        
        if(!fintersect.tetrahedronIntersect(atet) ) {
            continue;
        }
        
        Vector3F tcen = atet.getCenter();
        fintersect.closestToPoint(tcen);
        
        Vector3F pop = fintersect.closestToPointPoint();
        
        glColor3f(.01f, .3f, .2f);
        glVertex3fv((const float *)&tcen );
        glVertex3fv((const float *)&pop );
        glColor3f(.01f, .5f, .4f);
        glVertex3fv((const float *)&pop );
        Vector3F pnm = pop + fintersect.closestToPointNormal();
        glVertex3fv((const float *)&pnm );
    }
    glEnd();
    
}

void GLWidget::drawGround()
{
    glBegin(GL_LINES);
    m_sels.begin();
    while(!m_sels.end() ) {
        const int & k = m_sels.key();
        const cvx::Triangle * t = m_ground[k];
        
        glVertex3fv((const float *)&t->X(0) );
        glVertex3fv((const float *)&t->X(1) );
        glVertex3fv((const float *)&t->X(1) );
        glVertex3fv((const float *)&t->X(2) );
        glVertex3fv((const float *)&t->X(2) );
        glVertex3fv((const float *)&t->X(0) );
        
        m_sels.next();
    }
    glEnd();
}

void GLWidget::drawGridEdges()
{
    TetraGridEdgeMap<MesherT::GridT > & edges = m_mesher.gridEdges();
    
    glBegin(GL_LINES);
    edges.begin();
    while(!edges.end() ) {
        const sdb::Coord2 & k = edges.key();
        
        glVertex3fv((const float *)&m_grd->pos(k.x) );
        glVertex3fv((const float *)&m_grd->pos(k.y) );
        
        edges.next();
    }
    glEnd();

}

void GLWidget::drawField()
{
    m_fieldDrawer->drawEdge(m_mesher.field() );
    m_fieldDrawer->drawNode(m_mesher.field() );
}

void GLWidget::drawSolidGrid()
{
    getDrawer()->m_surfaceProfile.apply();
	
        glEnable(GL_CULL_FACE);
    glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	cvx::Tetrahedron atet;
    const int n = m_grd->numCells();
    for(int i=0;i<n;++i) {
        m_grd->getCell(atet, i);
        drawAShrinkSolidTetrahedron(atet, .67f);
    }
    
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void GLWidget::drawWiredGrid()
{
    glEnableClientState(GL_VERTEX_ARRAY);
	cvx::Tetrahedron atet;
    const int n = m_grd->numCells();
    for(int i=0;i<n;++i) {
        m_grd->getCell(atet, i);
        drawAWireTetrahedron(atet);
    }
    
    glDisableClientState(GL_VERTEX_ARRAY);

}

void GLWidget::drawTriangulation()
{
    int ntri = m_mesher.numFrontTriangles();

    Vector3F * trips = new Vector3F[ntri * 3];
    m_mesher.extractFrontTriangles(trips);
    
     getDrawer()->m_wireProfile.apply();
     getDrawer()->setColor(1,.7,0);
    glEnableClientState(GL_VERTEX_ARRAY);
	
    glVertexPointer(3, GL_FLOAT, 0, (GLfloat*)trips );
    glDrawArrays(GL_TRIANGLES, 0, ntri * 3);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    
    delete[] trips;
}

void GLWidget::clientSelect(Vector3F & origin, Vector3F & ray, Vector3F & hit)
{
}
//! [9]

void GLWidget::clientDeselect()
{
}

//! [10]
void GLWidget::clientMouseInput(Vector3F & stir)
{
}

void GLWidget::keyPressEvent(QKeyEvent *e)
{
	switch (e->key()) {
		case Qt::Key_M:
			//m_scene->progressForward();
			break;
		case Qt::Key_N:
			//m_scene->progressBackward();
			break;
		default:
			break;
	}
	Base3DView::keyPressEvent(e);
}

void GLWidget::keyReleaseEvent(QKeyEvent *event)
{
	Base3DView::keyReleaseEvent(event);
}
	