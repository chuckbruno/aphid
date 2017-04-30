#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <Base3DView.h>

class DrawNp;
class ContactThread;
class GLWidget : public Base3DView
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();
public slots:
	
signals:

protected:
    virtual void clientInit();
    virtual void clientDraw();
	virtual void clientSelect(QMouseEvent *event);
    virtual void clientDeselect(QMouseEvent *event);
    virtual void clientMouseInput(QMouseEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);
	
private:
    void drawTetra();
    
private:
    ContactThread * m_solver;
	
    DrawNp * m_dbgDraw;
	
private slots:
    
};
//! [3]

#endif
