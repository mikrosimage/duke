/*
 * Allocator.h
 *
 *  Created on: Dec 4, 2012
 *      Author: Guillaume Chatelet
 */

#ifndef ALLOCATOR_H_
#define ALLOCATOR_H_

#include <duke/memory/alloc/PageSize.h>
#include <duke/utils/NonCopyable.h>

#include <cstddef> // size_t
#include <cstdlib>

#ifndef __APPLE__
#include <malloc.h>
#endif

/**
 * Allocator interface to alloc and free memory
 */
struct Allocator: public NonCopyable {
	virtual ~Allocator() {
	}
	virtual void* malloc(const size_t size) const = 0;
	virtual void free(void* ptr) const = 0;
	virtual const char* name() const = 0;
	virtual size_t alignment() const = 0;
};

/**
 * Convenient method to create shared typed memory from
 * an allocator. The memory will be freed ( or returned
 * back to pool ) automagically.
 *
 * Allocator allocator;
 * auto pMemory = make_shared_memory<char>(1000, allocator);
 */
#include <functional>
#include <memory>
template<typename T>
std::shared_ptr<T> make_shared_memory(size_t size, Allocator &allocator) {
	using namespace std;
	return shared_ptr<T>(reinterpret_cast<T*>(allocator.malloc(size * sizeof(T))), bind(&Allocator::free, ref(allocator), placeholders::_1));
}

/**
 * Special purpose allocator
 */

// number of slots allocated at once by the ChunkAllocator
#ifndef CHUNK_ALLOCATOR_BLOCKS
#define CHUNK_ALLOCATOR_BLOCKS 8
#endif

struct BigAlignedBlock: public Allocator {
	BigAlignedBlock();
	virtual ~BigAlignedBlock();
	virtual void* malloc(const size_t size) const;
	virtual void free(void* ptr) const;
	virtual const char* name() const {
		return "BigAlignedBlock";
	}
	virtual size_t alignment() const {
		return PAGE_SIZE;
	}
private:
	struct BigAlignedBlockImpl;
	BigAlignedBlockImpl *pImpl;
};

/**
 * Simple Malloc
 */
struct Malloc: public Allocator {
	virtual void* malloc(const size_t size) const {
		return std::malloc(size);
	}
	virtual void free(void* pBuffer) const {
		std::free(pBuffer);
	}
	virtual size_t alignment() const {
		return 1;
	}
	virtual const char* name() const {
		return "Malloc";
	}
};

/**
 * Aligned allocator
 */
struct AlignedMalloc: public Allocator {
	virtual void* malloc(const size_t size) const {
#ifdef _WIN32
		return __mingw_aligned_malloc(size, aligned);
#elif defined(__APPLE__)
		return std::malloc( size ); // no memalign on macOSX
#else
		return memalign(PAGE_SIZE, size);
#endif
	}
	virtual void free(void* pBuffer) const {
#ifdef _WIN32
		__mingw_aligned_free(pBuffer);
#else
		std::free(pBuffer);
#endif
	}
	virtual size_t alignment() const {
		return PAGE_SIZE;
	}
	virtual const char* name() const {
		return "AlignedMalloc";
	}
};

/**
 * For completeness New allocator
 * NB as 'new' is implemented on top
 * of malloc it must be slower
 */
struct New: public Allocator {
	virtual void* malloc(const size_t size) const {
		return new char[size];
	}
	virtual void free(void* pBuffer) const {
		delete[] reinterpret_cast<char*>(pBuffer);
	}
	virtual size_t alignment() const {
		return 1;
	}
	virtual const char* name() const {
		return "New";
	}
};

// TODO virtual alloc also exists on linux : valloc()
// actually equivalent to memalign(sysconf(_SC_PAGESIZE),size)
/**
 * Virtual memory allocator
 */
#ifdef _WIN32
#include <windows.h>

struct Virtual : public Allocator {
	virtual void* malloc(const size_t size) const {
		return ::VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE); // aligned 4KB
	}
	virtual void free(void* pBuffer) const {
		::VirtualFree(pBuffer, 0, MEM_RELEASE);
	}
	virtual size_t alignment() const {
		return PAGE_SIZE;
	}
	virtual const char* name() const {
		return "Virtual";
	}
};
#endif

#endif /* ALLOCATOR_H_ */
