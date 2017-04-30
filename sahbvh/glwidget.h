#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <Base3DView.h>
class SahWorldInterface;
class CudaDynamicWorld;
class WorldThread;
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
    void stopPhysics();
    void startPhysics();
private:
    CudaDynamicWorld * m_world;
    SahWorldInterface * m_interface;
    WorldThread * m_thread;
    bool m_isPhysicsRunning;
private slots:
    void simulate();
signals:
    void updatePhysics();

};
//! [3]

#endif
