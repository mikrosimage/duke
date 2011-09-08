#include "BigAlignedBlockAllocator.h"

#include "PageSize.h"

#include <cstdio>
#include <cstring>
#include <cassert>

#ifndef __APPLE__
#include <malloc.h>
#endif


#include <iostream>
const std::string HEADER("[BigAlignedBlockAllocator] ");

// constant for block allocation
const unsigned char ChunkAllocatorBlocks = CHUNK_ALLOCATOR_BLOCKS;

struct ChunkAllocator; // forward declaring

// Sentinel code
struct Sentinel {
    unsigned int magic;
    ChunkAllocator* pAllocator;
};

inline static Sentinel& getSentinelFromBlockStart(void* pBlockStart) {
    return *(reinterpret_cast<Sentinel*> (pBlockStart) - 1);
}

inline static ChunkAllocator& getBigChunkFromBlockStart(void* pBlockStart) {
    const Sentinel &sentinel = getSentinelFromBlockStart(pBlockStart);
    assert( sentinel.magic == 0xDEADC0DE );
    return *sentinel.pAllocator;
}

// boundary helpers
inline static size_t lowerPage(size_t ptr) {
    return (ptr >> PAGE_SIZE_BITS) << PAGE_SIZE_BITS;
}

inline static size_t upperPage(size_t ptr) {
    return ((ptr >> PAGE_SIZE_BITS) + 1) << PAGE_SIZE_BITS;
}

inline static bool alignedToPage(size_t ptr) {
    return ptr == lowerPage(ptr);
}

// size helpers
inline static size_t getChunkSize(size_t chunkSize) {
    // each chunk as a sentinel
    const size_t realChunkSize = chunkSize + sizeof(Sentinel);
    return alignedToPage(realChunkSize) ? realChunkSize : upperPage(realChunkSize);
}

inline static size_t getAllocationSize(size_t chunkSize) {
    const size_t allAlignedChunkSize = getChunkSize(chunkSize) * ChunkAllocatorBlocks;
    // adding one PAGE_SIZE to ensure we can offset and give aligned data
    const size_t allocationSize = allAlignedChunkSize + PAGE_SIZE;
    return allocationSize;
}

/**
 * Reserve CHUNK blocks of contiguous memory aligned on PAGE_SIZE
 * you can then allocate and deallocate CHUNK blocks of size ChunkSize
 */
struct ChunkAllocator {
public:
    const size_t m_ChunkSize;

private:
    unsigned char m_AvailableSlots;
    char* m_pData;
    bool m_UsedSlot[ChunkAllocatorBlocks];
    void* m_SlotData[ChunkAllocatorBlocks];

private:
    inline unsigned char findFreeSlot() const {
        unsigned char i = 0;
        for (; i < ChunkAllocatorBlocks; ++i)
            if (m_UsedSlot[i] == false)
                return i;
        assert(false);
        return i;
    }

    inline unsigned char findSlot(void* pData) const {
        unsigned char i = 0;
        for (; i < ChunkAllocatorBlocks; ++i)
            if (m_SlotData[i] == pData)
                return i;
        assert(false);
        return i;
    }

public:
    ChunkAllocator(size_t chunkSize) :
        m_ChunkSize(chunkSize), m_AvailableSlots(ChunkAllocatorBlocks), m_pData((char*) malloc(getAllocationSize(chunkSize))) {
        assert( PAGE_SIZE == (size_t)getPageSize() );
        memset(m_UsedSlot, false, ChunkAllocatorBlocks);
        char* pBlockStart = reinterpret_cast<char*> (upperPage(reinterpret_cast<size_t> (m_pData)));
        const size_t realChunkSize = getChunkSize(chunkSize);
        for (int i = 0; i < ChunkAllocatorBlocks; ++i) {
            m_SlotData[i] = pBlockStart;
            Sentinel &sentinel = getSentinelFromBlockStart(pBlockStart);
            sentinel.pAllocator = this;
            sentinel.magic = 0xDEADC0DE;
            pBlockStart += realChunkSize;
        }
        std::cout << HEADER << "Allocating " << (unsigned int)ChunkAllocatorBlocks << " blocks of size " << chunkSize << std::endl;
    }

    ~ChunkAllocator() {
        ::free(m_pData);
    }

    inline bool isNotFull() const {
        return m_AvailableSlots != 0;
    }

    inline bool isNotEmpty() const {
        return m_AvailableSlots != ChunkAllocatorBlocks;
    }

    inline bool contains(void* ptr) const {
        return getSentinelFromBlockStart(ptr).pAllocator == ptr;
    }

    void* allocate() {
        assert(isNotFull());
        const unsigned char index = findFreeSlot();
        --m_AvailableSlots;
        m_UsedSlot[index] = true;
        return m_SlotData[index];
    }

    void free(void* pData) {
        assert(isNotEmpty());
        m_UsedSlot[findSlot(pData)] = false;
        ++m_AvailableSlots;
    }
};

#include <boost/ptr_container/ptr_list.hpp>

/**
 * Maintains a list of dynamically allocated Chunks
 */
struct ChunkManager {
    const size_t m_ChunkSize;
private:
    typedef ChunkAllocator PCHUNKS;
    typedef boost::ptr_list<PCHUNKS> VECTOR;
    typedef VECTOR::iterator ITR;
    VECTOR m_Chunks;

public:
    ChunkManager(const size_t chunkSize) :
        m_ChunkSize(chunkSize) {
    }
    void* allocate() {
        ITR current = m_Chunks.begin();
        const ITR end = m_Chunks.end();
        for (; current != end; ++current)
            if (current->isNotFull())
                break;
        if (current == end) { // time to allocate a new chunk
            // putting chunk front so next time we will find it right away
            m_Chunks.push_front(new ChunkAllocator(m_ChunkSize));
            current = m_Chunks.begin();
        }
        return current->allocate();
    }
};

/**
 * BigAlignedBlockAllocator implementation with PImpl idiom
 */

#include <boost/ptr_container/ptr_map.hpp>

struct BigAlignedBlockAllocatorImpl {
    typedef boost::ptr_map<size_t, ChunkManager> MAP;
    typedef MAP::iterator ITR;
    MAP m_SizeToManager;
};

BigAlignedBlockAllocator::BigAlignedBlockAllocator() :
    pImpl(new BigAlignedBlockAllocatorImpl()) {
}

BigAlignedBlockAllocator::~BigAlignedBlockAllocator() {
    delete pImpl;
}

void* BigAlignedBlockAllocator::malloc(size_t size) {
    BigAlignedBlockAllocatorImpl::MAP &map = pImpl->m_SizeToManager;
    BigAlignedBlockAllocatorImpl::ITR itr = map.find(size);
    if (itr == map.end())
        itr = map.insert(size, new ChunkManager(size)).first;
    return itr->second->allocate();
}

void BigAlignedBlockAllocator::free(void* pData) {
    getBigChunkFromBlockStart(pData).free(pData);
}
