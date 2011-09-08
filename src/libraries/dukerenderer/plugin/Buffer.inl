/*
 * Buffer.cpp
 *
 *  Created on: 11 mai 2010
 *      Author: Guillaume Chatelet
 */

#include "Buffer.h"
#include <cassert>

template<class T>
Buffer<T>::Buffer(IBufferBase* pBuffer) :
	m_pBufferBase(pBuffer) {
}

template<class T>
T* Buffer<T>::lock(unsigned long offset, unsigned long size, unsigned long flags) const {
	return reinterpret_cast<T*>(m_pBufferBase->lock(offset * sizeof(T), size * sizeof(T), flags));
}

template<class T>
void Buffer<T>::unlock() const {
	m_pBufferBase->unlock();
}

template<class T>
void Buffer<T>::fill(const T* data, std::size_t count) const {
    assert(data != NULL);

    T* MyData = lock();
    std::copy(data, data + count, MyData);
    unlock();
}

template<class T>
void Buffer<T>::release() {
	m_pBufferBase.reset();
}

template<class T>
const IBufferBase* Buffer<T>::getBuffer() const {
	return m_pBufferBase.get();
}

template<class T>
unsigned long Buffer<T>::size() const {
	if(m_pBufferBase==NULL)
		return 0;
	return m_pBufferBase->size();
}