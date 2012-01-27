#include "MappedFileIO.h"
#include <iostream>
#include <fstream>
#include <dukeengine/memory/MemoryUtils.h>
#include <boost/iostreams/device/mapped_file.hpp>

using namespace mikrosimage::alloc;

MappedFileIO::MappedFileIO(Allocator* pAllocator) :
                FileIO(pAllocator) {
    assert( m_pAllocator != NULL);
}

MappedFileIO::~MappedFileIO() {
}

MemoryBlockPtr MappedFileIO::read(const char* filename) {
    using namespace boost::iostreams;

    try {
        mapped_file file(filename, mapped_file::readonly);
        const size_t size(file.size());
        MemoryBlockPtr pMemoryBlock(new MemoryBlock(m_pAllocator, size));
        memcpy(pMemoryBlock->getPtr<void>(), file.const_data(), size);
        return pMemoryBlock;
    } catch (std::ios_base::failure &e) {
        return MemoryBlockPtr();
    }
}

