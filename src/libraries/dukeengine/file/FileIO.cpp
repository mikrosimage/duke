#include "FileIO.h"

using namespace mikrosimage::alloc;

FileIO::FileIO( Allocator* pAllocator ) : m_pAllocator( pAllocator ) {}

FileIO::~FileIO() {}

Allocator* FileIO::allocator() const
{
	return m_pAllocator;
}

