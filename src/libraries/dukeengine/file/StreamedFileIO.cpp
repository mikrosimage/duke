/*
 * StreamedFileIO.cpp
 *
 *  Created on: 5 mai 2010
 *      Author: Guillaume Chatelet
 */

#include "StreamedFileIO.h"
#include <iostream>
#include <fstream>

using namespace mikrosimage::alloc;

StreamedFileIO::StreamedFileIO( Allocator* pAllocator ) : FileIO( pAllocator ) {}

StreamedFileIO::~StreamedFileIO() {}

MemoryBlockPtr StreamedFileIO::read( const char* filename )
{
	std::ifstream infile( filename, std::ios::binary | std::ios::ate );

	if( !infile.is_open() )
		return MemoryBlockPtr();

	const size_t size = infile.tellg();
	infile.seekg( 0, std::ios::beg );

	MemoryBlockPtr pMemoryBlock( new MemoryBlock( m_pAllocator, size ) );
	infile.read( pMemoryBlock->getPtr<char>(), size );
	return pMemoryBlock;
}

