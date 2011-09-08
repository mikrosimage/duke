/*
 * Allocators.h
 *
 *  Created on: 20 avr. 2010
 *      Author: Guillaume Chatelet
 */

#ifndef ALLOCATORS_H_
#define ALLOCATORS_H_

#include "PageSize.h"
#include "BigAlignedBlockAllocator.h"

#include <cstddef>
#include <cstdlib>

#ifndef __APPLE__
#include <malloc.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#include <boost/noncopyable.hpp>

namespace mikrosimage {

namespace alloc {

struct AlignedMalloc {
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    enum {
        aligned = PAGE_SIZE
    };

    static char* malloc(const size_t size) {
        return reinterpret_cast<char*> (
#ifdef _WIN32
        __mingw_aligned_malloc(size, aligned)
#elif defined(__APPLE__)
        std::malloc( size ) // no memalign on macOSX
#else
        memalign(PAGE_SIZE, size)
#endif
        );
    }

    static void free(char* const pBuffer) {
#ifdef _WIN32
        __mingw_aligned_free(pBuffer);
#else
        std::free(pBuffer);
#endif
    }

};

struct Malloc {
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    enum {
        aligned = 1
    };

    static char* malloc(const size_t size) {
        return reinterpret_cast<char*> (std::malloc(size));
    }

    static void free(char* const pBuffer) {
        std::free(pBuffer);
    }

};

struct New {
    enum {
        aligned = 1
    };

    static char* malloc(const size_t size) {
        return new char[size];
    }

    static void free(char* const pBuffer) {
        delete[] pBuffer;
    }

};

// TODO virtual alloc also exists on linux : valloc()
// actually equivalent to memalign(sysconf(_SC_PAGESIZE),size)
#ifdef _WIN32
struct Virtual {
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    enum {
        aligned = PAGE_SIZE
    };

    static char* malloc(const size_t size) {
        return reinterpret_cast<char*> (::VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE)); // aligned 4KB
    }

    static void free(char* const pBuffer) {
        ::VirtualFree(pBuffer, 0, MEM_RELEASE);
    }
};
#endif

struct Allocator : public boost::noncopyable {
    Allocator();
    virtual ~Allocator() = 0;
    virtual char* malloc(const size_t size) = 0;
    virtual void free(char* const ptr) = 0;
    virtual size_t alignment() const = 0;
};

template<typename ALLOC>
struct AllocatorImpl : public Allocator {
    virtual char* malloc(const size_t size) {
        return ALLOC::malloc(size);
    }

    virtual void free(char* const ptr) {
        ALLOC::free(ptr);
    }

    virtual size_t alignment() const {
        return ALLOC::aligned;
    }
};

typedef AllocatorImpl<AlignedMalloc> AlignedMallocAllocator;
typedef AllocatorImpl<Malloc> MallocAllocator;
typedef AllocatorImpl<New> NewAllocator;
#ifdef _WIN32
typedef AllocatorImpl<Virtual> VirtualAllocator;
#endif

struct BigAlignedBlockAllocator : public Allocator, private ThreadSafeBigAlignedBlockAllocator {
    virtual ~BigAlignedBlockAllocator() {
    }
    virtual char* malloc(const size_t size) {
        return (char*)ThreadSafeBigAlignedBlockAllocator::malloc(size);
    }
    virtual void free(char* const ptr) {
        return ThreadSafeBigAlignedBlockAllocator::free(ptr);
    }
    virtual size_t alignment() const {
        return PAGE_SIZE;
    }
};

} // namespace alloc

} // namespace mikrosimage

#endif /* ALLOCATORS_H_ */
