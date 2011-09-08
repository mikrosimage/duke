#include "IBufferBase.h"

IBufferBase::IBufferBase( unsigned long count )
	: m_uCount( count ) {}

IBufferBase::~IBufferBase() {}

unsigned long IBufferBase::size() const
{
	return m_uCount;
}

