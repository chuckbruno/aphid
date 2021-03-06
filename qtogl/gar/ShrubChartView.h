/*
 *  ShrubChartView.h
 *  garden
 *
 *  Created by jian zhang on 3/30/17.
 *  Copyright 2017 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef GAR_SHRUB_CHART_VIEW_H
#define GAR_SHRUB_CHART_VIEW_H

#include <QGraphicsView>

QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QGraphicsItem;
class QPixmap;
class QPoint;
QT_END_NAMESPACE

class GlyphConnection;

class ShrubChartView : public QGraphicsView
{
    Q_OBJECT

public:
    ShrubChartView(QGraphicsScene * scene, QWidget * parent = 0);
	
protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
	
private:
	void addGlyphPiece(const QPoint & pieceTypGrp, 
		const QPixmap & px,
		const QPoint & pos);
    void processSelect(const QPoint & pos);
	bool isItemPort(const QGraphicsItem *item) const;
	bool isOutgoingPort(const QGraphicsItem *item) const;
	bool isIncomingPort(const QGraphicsItem *item) const;
	
	enum Mode {
		mNone = 0,
		mPanView = 1,
		mMoveItem = 2,
		mConnectItems = 3
	}; 
	
	Mode m_mode;
	QGraphicsItem * m_selectedItem;
	GlyphConnection * m_selectedConnection;
	QPoint m_lastMosePos;
	
};

#endif