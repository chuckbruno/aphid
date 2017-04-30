#ifndef CUDATEXTURE_H
#define CUDATEXTURE_H

/*
 *  CUDABuffer.h
 *  brdf
 *
 *  Created by jian zhang on 9/30/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <gl_heads.h>
#include <cuda_runtime_api.h>
class CudaTexture {
public:
	CudaTexture();
	virtual ~CudaTexture();
	
	void create(unsigned width, unsigned height, int pixelDepth, bool isHalf);
	void destroy();
	
	void copyFrom(void * src, unsigned size);
	
	void bind();
	
	GLuint * texture();
	
private:
	GLuint m_texture;
	unsigned m_width, m_height, m_pixelDepth;
	cudaGraphicsResource * _cuda_tex_resource;
	bool m_isHalf;
};

#endif        //  #ifndef CUDATEXTURE_H

