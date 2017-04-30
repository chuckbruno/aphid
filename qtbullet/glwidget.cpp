/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>
#include <QtOpenGL>

#include <math.h>

#include "glwidget.h"


//! [0]
GLWidget::GLWidget(QWidget *parent) : Base3DView(parent)
{
    _dynamics = new DynamicsSolver();
	_dynamics->initPhysics();
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(simulate()));
	timer->start(30);
}
//! [0]

//! [1]
GLWidget::~GLWidget()
{
}
//! [1]

DynamicsSolver* GLWidget::getSolver()
{
    return _dynamics;
}

//! [7]
void GLWidget::clientDraw()
{
    _dynamics->renderWorld();
}
//! [7]

//! [9]
void GLWidget::clientSelect(Vector3F & origin, Vector3F & ray, Vector3F & hit)
{
    _dynamics->selectByRayHit(origin, ray, hit);
    if(_dynamics->getInteractMode() == DynamicsSolver::ToggleLock) 
        _dynamics->toggleMassProp();
}
//! [9]

void GLWidget::clientDeselect()
{
    _dynamics->removeSelection();
    if(_dynamics->getInteractMode() == DynamicsSolver::RotateJoint) {
        _dynamics->removeTorque();
    }
}

//! [10]
void GLWidget::clientMouseInput(Vector3F & stir)
{
    if(!_dynamics->hasActive())
        return;
    if(_dynamics->getInteractMode() == DynamicsSolver::TranslateBone)
        _dynamics->addImpulse(stir);
    //else 
    //    _dynamics->(addTorquestir);
}
//! [10]

void GLWidget::simulate()
{
    update();
    _dynamics->simulate();
}


