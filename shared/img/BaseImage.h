/*
 *  BaseImage.h
 *  arum
 *
 *  Created by jian zhang on 9/1/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once
#include <string>

namespace aphid {

class BaseImage {

		bool m_isValid;
    	int m_imageWidth, m_imageHeight;
		std::string m_fileName;
		
public:
	enum IFormat {
		FUnknown = 0,
		FExr = 1
	};
	
	enum ChannelRank {
		None = 0,
		RED = 1,
		RGB = 3,
		RGBA = 4,
		RGBAZ = 5
	};
	
	BaseImage();
	BaseImage(const std::string & filename);
	virtual ~BaseImage();
	
	bool read(const std::string & filename);
	
	const bool & isValid() const;
	const std::string & fileName() const;
	
	virtual IFormat formatName() const;
	std::string formatNameStr() const;
	std::string channelRankStr() const;
	virtual void allWhite();
	virtual void allBlack();
	
	void setChannelRank(ChannelRank x);
	ChannelRank channelRank() const;
    void setWidth(int x);
    void setHeight(int x);
	int getWidth() const;
	int getHeight() const;
	const float aspectRation() const;
	int pixelLoc(float s, float t, bool flipV, int pixelRank) const;

	virtual void sample(float u, float v, int count, float * dst) const;
	virtual float sampleRed(float u, float v);
	virtual float sampleRed(float u, float v) const;
	virtual void setRed(float u, float v, float red);
	virtual void applyMask(BaseImage * another);
	bool isRGBAZ() const;
	void verbose() const;
	
protected:
	virtual bool readImage(const std::string & filename);
	
private:
    ChannelRank m_channelRank;
	
};

}