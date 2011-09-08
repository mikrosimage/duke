/*
 * IFactory.cpp
 *
 *  Created on: 11 mai 2010
 *      Author: Guillaume Chatelet
 */

template <class T>
inline Buffer<T> IFactory::createVertexBuffer(unsigned long size, unsigned long flags, const T* data) const
{
    Buffer<T> buffer(createVB(size, sizeof(T), flags));
    if (data)
    	buffer.fill(data, size);

    return buffer;
}

template <class T>
inline Buffer<T> IFactory::createIndexBuffer(unsigned long size, unsigned long flags, const T* data) const
{
    Buffer<T> buffer(createIB(size, sizeof(T), flags));
    if (data)
        buffer.fill(data, size);

    return buffer;
}