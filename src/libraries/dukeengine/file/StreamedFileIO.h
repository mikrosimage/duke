#ifndef STREAMEDFILEIO_H_
#define STREAMEDFILEIO_H_

#include "FileIO.h"

class StreamedFileIO : public FileIO
{
public:
	StreamedFileIO( ::mikrosimage::alloc::Allocator* pAllocator );
	virtual ~StreamedFileIO();

	virtual MemoryBlockPtr read( const char* filename );
};

#endif /* STREAMEDFILEIO_H_ */
