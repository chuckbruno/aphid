#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <Base3DView.h>
class FEMTetrahedronMesh;
class SolverThread;
class GLWidget : public Base3DView
{
    Q_OBJECT

public:

    GLWidget(QWidget *parent = 0);
    ~GLWidget();
	
protected:
	virtual void clientInit();
    virtual void clientDraw();
    virtual void clientSelect(Vector3F & origin, Vector3F & ray, Vector3F & hit);
    virtual void clientDeselect();
    virtual void clientMouseInput(Vector3F & stir);
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void keyReleaseEvent(QKeyEvent *event);
    
private:
	void drawMesh(FEMTetrahedronMesh * mesh);
private:
    SolverThread * m_solver;
	
private slots:
    void simulate();
};
//! [3]

#endif
