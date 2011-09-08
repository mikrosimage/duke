/*
 * IBufferBase.h
 *
 *  Created on: 11 mai 2010
 *      Author: Guillaume Chatelet
 */

#ifndef IBUFFERBASE_H_
#define IBUFFERBASE_H_

#include <boost/utility.hpp>

class IBufferBase : public boost::noncopyable
{
public: IBufferBase( unsigned long count );
	virtual ~IBufferBase() = 0;

	virtual void* lock( unsigned long offset, unsigned long size, unsigned long flags ) = 0;
	virtual void  unlock()                                                              = 0;
	unsigned long size() const;

protected:
	const unsigned long m_uCount;
};

#endif /* IBUFFERBASE_H_ */
