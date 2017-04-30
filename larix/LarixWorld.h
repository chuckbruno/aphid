/*
 *  LarixWorld.h
 *  larix
 *
 *  Created by jian zhang on 7/24/15.
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once
#include <AWorld.h>
#include <Vector3F.h>
class ATetrahedronMesh;
class APointCloud;
class AdaptiveField;
class H5FieldIn;
class H5FieldOut;
class TypedBuffer;
class BaseBuffer;

class LarixWorld : public AWorld {
public:
	LarixWorld();
	virtual ~LarixWorld();
	
	// override aworld
    virtual void progressFrame();
    
    void setTetrahedronMesh(ATetrahedronMesh * m);
    ATetrahedronMesh * tetrahedronMesh() const;
	
	void setPointCloud(APointCloud * pc);
	APointCloud * pointCloud() const;
	
	void setField(AdaptiveField * g);
	AdaptiveField * field() const;
	
	void setFieldTranslate(const Vector3F & v);

    bool hasSourceP() const;
    TypedBuffer * sourceP();
    void setSourceP(TypedBuffer * x);

    int cacheRangeMin() const;
    int cacheRangeMax() const;
    void beginCache();
    int currentCacheFrame() const;
	
	bool setFileOut(const std::string & fileName);
    bool isCachingFinished() const;
protected:
    bool checkSourceField();
    void setCacheRange();
private:
    ATetrahedronMesh * m_mesh;
	APointCloud * m_cloud;
	AdaptiveField * m_field;
	H5FieldIn * m_sourceFile;
    TypedBuffer * m_sourceP;
    H5FieldOut * m_cacheFile;
    bool m_isCachingFinished;
};