/*
 * VirtualMemoryReserver.h
 *
 *  Created on: 16 avr. 2010
 *      Author: Guillaume Chatelet
 */

#ifndef VIRTUALMEMORYRESERVER_H_
#define VIRTUALMEMORYRESERVER_H_

#include <boost/utility.hpp>

class VirtualMemoryReserver : private ::boost::noncopyable
{
private:
	size_t m_uReservedSize;
	size_t m_uPageSize;

public:
	VirtualMemoryReserver( const size_t size );
	~VirtualMemoryReserver();

	size_t getReservedSize() const { return m_uReservedSize; }
	size_t getPageSize() const     { return m_uPageSize; }
};

#endif /* VIRTUALMEMORYRESERVER_H_ */
