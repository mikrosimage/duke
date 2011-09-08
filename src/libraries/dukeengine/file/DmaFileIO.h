#ifndef DMAFILEIO_H_
#define DMAFILEIO_H_

#include "FileIO.h"

class DmaFileIO : public FileIO
{
public:
	DmaFileIO( ::mikrosimage::alloc::Allocator* pAllocator );
	virtual ~DmaFileIO();

	virtual MemoryBlockPtr read( const char* filename );
};

#endif /* DMAFILEIO_H_ */
