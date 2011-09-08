/*
 * IBufferBase.cpp
 *
 *  Created on: 11 mai 2010
 *      Author: Guillaume Chatelet
 */

#include "IBufferBase.h"

IBufferBase::IBufferBase( unsigned long count )
	: m_uCount( count ) {}

IBufferBase::~IBufferBase() {}

unsigned long IBufferBase::size() const
{
	return m_uCount;
}

