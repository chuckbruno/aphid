/*
 *  CUDAProgram.h
 *  
 *
 *  Created by jian zhang on 10/1/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once
#include <CUDABuffer.h>
class CUDAProgram {
public:
	CUDAProgram();
	virtual ~CUDAProgram();
	
	virtual void run(CUDABuffer * buffer);
	
	void map(CUDABuffer * buffer, void ** p);
	void unmap(CUDABuffer * buffer);
	
	void calculateDim(unsigned count, unsigned & w, unsigned & h);

};