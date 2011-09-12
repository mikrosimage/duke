/*
 * DmaFileIO.cpp
 *
 *  Created on: 5 mai 2010
 *      Author: Guillaume Chatelet
 */

#ifdef _WIN32

#include "DmaFileIO.h"
#include <dukeengine/memory/MemoryUtils.h>
#include <cassert>
#include <windows.h>

using namespace ::mikrosimage::alloc;

DmaFileIO::DmaFileIO( Allocator* pAllocator )
	: FileIO( pAllocator )
{
	assert( m_pAllocator->alignment() == PAGE_SIZE );
}

DmaFileIO::~DmaFileIO() {}

MemoryBlockPtr DmaFileIO::read( const char* filename )
{
	HANDLE hFileHandle;

	if( ( hFileHandle = CreateFile( filename, //
	                                GENERIC_READ, //
	                                0, //
	                                NULL, //
	                                OPEN_EXISTING, //
	                                FILE_FLAG_NO_BUFFERING | FILE_FLAG_SEQUENTIAL_SCAN, //
	                                NULL ) //
	      ) == INVALID_HANDLE_VALUE )
		return MemoryBlockPtr();

	DWORD fileSizeHighOrder;
	DWORD fileSizeLowOrder;
	if( ( fileSizeLowOrder = GetFileSize( hFileHandle, &fileSizeHighOrder ) ) == INVALID_FILE_SIZE )
		return MemoryBlockPtr();

	if( fileSizeHighOrder != 0 )
		return MemoryBlockPtr();

	// we should roundUp the size to the next sector size
	// sector size is typically 512 but here we are aligning
	// to page size which is 4096 on x86 (8192 for Itanium processor family)
	// http://msdn.microsoft.com/en-us/library/cc644950(v=VS.85).aspx
	const size_t fileSize = roundUp<DWORD, PAGE_SIZE>( fileSizeLowOrder ); // aligning the size to read
	MemoryBlockPtr pMemoryBlock( new MemoryBlock( m_pAllocator, fileSize ) );
	DWORD readBytes;
	if( ReadFile( hFileHandle, pMemoryBlock->getPtr<void>(), fileSize, &readBytes, NULL ) == 0 )
		return MemoryBlockPtr();
	CloseHandle( hFileHandle );

	return pMemoryBlock;
}

#endif
