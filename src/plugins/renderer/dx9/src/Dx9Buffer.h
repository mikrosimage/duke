/*
 * Dx9Buffer.h
 *
 *  Created on: 11 mai 2010
 *      Author: Guillaume Chatelet
 */

#ifndef DX9BUFFER_H_
#define DX9BUFFER_H_

#include "ScopedIUnknownPtr.h"
#include <renderer/plugin/common/IBufferBase.h>

template<typename T>
class Dx9Buffer : public IBufferBase {
public:
    Dx9Buffer(unsigned long count, T* buffer);
    virtual ~Dx9Buffer();

    T* getBuffer() const;

protected:
    virtual void* lock(unsigned long offset, unsigned long size, unsigned long flags);
    virtual void unlock();

    ScopedIUnknownPtr<T> m_pBuffer;

private:
    Dx9Buffer(const Dx9Buffer<T>&);
};

struct IDirect3DVertexBuffer9;
struct IDirect3DIndexBuffer9;

typedef Dx9Buffer<IDirect3DVertexBuffer9> DX9VertexBuffer;
typedef Dx9Buffer<IDirect3DIndexBuffer9> DX9IndexBuffer;

#include "Dx9Buffer.inl"

#endif /* DX9BUFFER_H_ */
