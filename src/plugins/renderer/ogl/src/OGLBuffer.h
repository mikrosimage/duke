/*
 * OGLBuffer.h
 *
 *  Created on: 02 juin 2010
 *      Author: Nicolas Rondaud
 */

#ifndef OGLBUFFER_H_
#define OGLBUFFER_H_

#include <GL/glew.h>
#include <renderer/plugin/common/IBufferBase.h>

template<int T>
class OGLBuffer : public IBufferBase
{
public:
	OGLBuffer( unsigned long count, unsigned int buffer );
	virtual ~OGLBuffer();

	unsigned int getBuffer() const;

protected:
	virtual void* lock( unsigned long offset, unsigned long size, unsigned long flags );
	virtual void  unlock();

	GLuint m_pBuffer;
};

typedef OGLBuffer<GL_ARRAY_BUFFER_ARB> OGLVertexBuffer;
typedef OGLBuffer<GL_ELEMENT_ARRAY_BUFFER_ARB> OGLIndexBuffer;

#include "OGLBuffer.inl"

#endif /* OGLBUFFER_H_ */
