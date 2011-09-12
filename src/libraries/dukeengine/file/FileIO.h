#ifndef FILEIO_H_
#define FILEIO_H_

#include <dukeengine/memory/alloc/MemoryBlock.h>
#include <dukeengine/memory/alloc/Allocators.h>

class FileIO
{
protected:
	::mikrosimage::alloc::Allocator * const m_pAllocator;

public:
	FileIO( ::mikrosimage::alloc::Allocator* pAllocator );

	virtual ~FileIO() = 0;
	::mikrosimage::alloc::Allocator* allocator() const;
	virtual MemoryBlockPtr           read( const char* filename ) = 0;
};

#endif /* FILEIO_H_ */
