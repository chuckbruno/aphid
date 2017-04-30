/*
 *  voxWidget.h
 *  
 *
 *  Created by jian zhang on 4/9/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef VOX_Widget_H
#define VOX_Widget_H

#include <Base3DView.h>
#include <voxTest.h>

class VoxWidget : public aphid::Base3DView 
{
	Q_OBJECT
	
public:
    
    VoxWidget(QWidget *parent = 0);
    virtual ~VoxWidget();

protected:    
    virtual void clientInit();
    virtual void clientDraw();
	virtual void clientMouseInput(QMouseEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
	
private:
	void drawGrids();
	void drawTriangles();
	void drawFronts();
	
	
private:
	aphid::VoxTest m_test;
	
};
#endif