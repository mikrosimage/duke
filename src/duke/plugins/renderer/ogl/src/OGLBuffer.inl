#include "OGLBuffer.h"
#include "OGLEnum.h"

template<int T>
inline OGLBuffer<T>::OGLBuffer(unsigned long count, unsigned int buffer) :
	IBufferBase(count)
	, m_pBuffer(buffer)
{
}

template<int T>
OGLBuffer<T>::~OGLBuffer() {
	if(m_pBuffer)
		glDeleteBuffersARB(1, &m_pBuffer);
}

template<int T>
unsigned int OGLBuffer<T>::getBuffer() const {
	return m_pBuffer;
}

template<int T>
void* OGLBuffer<T>::lock(unsigned long offset, unsigned long size, unsigned long flags) {
	glBindBufferARB(T, m_pBuffer);
    unsigned char* buffer = reinterpret_cast<unsigned char*>(glMapBufferARB(T, OGLEnum::LockFlags(flags)));

    return buffer + offset;
}

template<int T>
void OGLBuffer<T>::unlock() {
	glUnmapBufferARB(T);
}
