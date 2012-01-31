#include "Dx9Buffer.h"
#include "DX9Enum.h"

template<typename T>
inline Dx9Buffer<T>::Dx9Buffer(unsigned long count, T* buffer) :
	IBufferBase(count), m_pBuffer(buffer) {
}

template<typename T>
Dx9Buffer<T>::~Dx9Buffer() {}

template<typename T>
T* Dx9Buffer<T>::getBuffer() const {
	return m_pBuffer;
}

template<typename T>
void* Dx9Buffer<T>::lock(unsigned long offset, unsigned long size, unsigned long flags) {
	void* data = NULL;
	m_pBuffer->Lock(offset, size, &data, Dx9Enum::LockFlags(flags));
	return data;
}

template<typename T>
void Dx9Buffer<T>::unlock() {
	m_pBuffer->Unlock();
}
