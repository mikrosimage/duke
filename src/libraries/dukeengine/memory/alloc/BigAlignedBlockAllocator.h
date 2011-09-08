/*
 * BigAlignedBlockAllocator.h
 *
 * This allocator is optimized for requesting quite always the same size.
 * The more different size you are requesting, the slower the allocation will
 * become.
 *
 * If size X is requested, CHUNK_ALLOCATOR_BLOCKS blocks of that size will be
 * created at once. So getting the CHUNK_ALLOCATOR_BLOCKS-1 other blocks
 * will almost be free.
 *
 * When free is called, the memory is recycled and ready to be reused. The free
 * operation is very fast. Allocation is indeed a bit slower because of the
 * various check or allocation that can occur.
 *
 * If no more than CHUNK_ALLOCATOR_BLOCKS is requested at the same time then
 * the memory allocator should be very fast, if one more block is requested
 * then another CHUNK_ALLOCATOR_BLOCKS blocks are created, doubling the further
 * capacity.
 *
 * As for vector memory cannot shrink back.
 *
 * All memory requested will be page aligned to allow very DMA transfer.
 *
 *  Created on: Jan 8, 2011
 *      Author: Guillaume Chatelet
 */

#ifndef BIGALIGNEDBLOCKALLOCATOR_H_
#define BIGALIGNEDBLOCKALLOCATOR_H_

// number of slots allocated at once by the ChunkAllocator
#ifndef CHUNK_ALLOCATOR_BLOCKS
#define CHUNK_ALLOCATOR_BLOCKS 8
#endif

#include <cstddef>
struct BigAlignedBlockAllocatorImpl;

class BigAlignedBlockAllocator {
    BigAlignedBlockAllocatorImpl *pImpl;
public:
    BigAlignedBlockAllocator();
    ~BigAlignedBlockAllocator();

    void* malloc(size_t size);
    void free(void* pData);
};

/**
 * Thread safe version
 */
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
class ThreadSafeBigAlignedBlockAllocator : public BigAlignedBlockAllocator {
    boost::mutex m_Mutex;
public:
    void* malloc(size_t size){
        boost::mutex::scoped_lock lock(m_Mutex);
        return BigAlignedBlockAllocator::malloc(size);
    }
    void free(void* pData){
        boost::mutex::scoped_lock lock(m_Mutex);
        BigAlignedBlockAllocator::free(pData);
    }
};

#endif /* BIGALIGNEDBLOCKALLOCATOR_H_ */
