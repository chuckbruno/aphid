/*
 *  SceneTreeParser.h
 *  mallard
 *
 *  Created by jian zhang on 1/18/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include "SceneTreeModel.h"
class MlScene;
class BaseShader;
class SceneTreeParser : public SceneTreeModel {
	Q_OBJECT
public:
	SceneTreeParser(const QStringList &headers, MlScene* scene, QObject *parent = 0);
    ~SceneTreeParser();
	
	void rebuild();
public slots:
	void receiveData(QWidget * editor);
signals:
	void cameraChanged();
protected:
	void addOptions(QList<SceneTreeItem*> & parents);
	void addCamera(QList<SceneTreeItem*> & parents);
	void addLights(QList<SceneTreeItem*> & parents);
	void addShaders(QList<SceneTreeItem*> & parents);
	void addFeatherShader(QList<SceneTreeItem*> & parents, BaseShader * s);
	void setupModelData(SceneTreeItem *parent);
    void updateScene(SceneTreeItem * item);
	void updateOptions(SceneTreeItem * item);
	void updateCamera(SceneTreeItem * item);
	void updateLights(SceneTreeItem * item);
	void updateShaders(SceneTreeItem * item);
	void updateFeatherShader(SceneTreeItem * item, BaseShader * s);
private:
	MlScene * m_scene;
};