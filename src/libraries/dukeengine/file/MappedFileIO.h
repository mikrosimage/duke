/*
 * MappedFileIO.h
 *
 *  Created on: 31 aout 2011
 *      Author: Nicolas Rondaud
 */

#ifndef MAPPEDFILEIO_H_
#define MAPPEDFILEIO_H_

#include "FileIO.h"

class MappedFileIO : public FileIO
{
public:
    MappedFileIO( ::mikrosimage::alloc::Allocator* pAllocator );
	virtual ~MappedFileIO();

public:
	virtual MemoryBlockPtr read( const char* filename );
};

#endif /* MAPPEDFILEIO_H_ */
