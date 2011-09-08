/*
 * Buffer.h
 *
 *  Created on: 11 mai 2010
 *      Author: Guillaume Chatelet
 */

#ifndef BUFFER_H_
#define BUFFER_H_

#include "IBufferBase.h"
#include <boost/shared_ptr.hpp>

template<class T>
class Buffer
{
public: Buffer( IBufferBase* pBuffer = NULL );

	T*                 lock( unsigned long offset = 0, unsigned long size = 0, unsigned long flags = 0 ) const;
	void               unlock() const;
	void               fill( const T* data, std::size_t count ) const;
	void               release();
	const IBufferBase* getBuffer() const;
	unsigned long      size() const;

private:
	boost::shared_ptr<IBufferBase> m_pBufferBase;
};

#include "Buffer.inl"

#endif /* BUFFER_H_ */
