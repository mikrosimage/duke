/*
 * MemoryBlock.h
 *
 *  Created on: 19 avr. 2010
 *      Author: Guillaume Chatelet
 */

#ifndef MEMORYBLOCK_H_
#define MEMORYBLOCK_H_

namespace mikrosimage {
namespace alloc {
struct Allocator;
} // namespace alloc
} // namespace mikrosimage

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <stdexcept>
#include <cassert>

class MemoryBlock;

class MemoryBlock : private ::boost::noncopyable {
protected:
    ::mikrosimage::alloc::Allocator * m_pAllocator;
    const size_t m_uSize;
    char* m_pBuffer;

public:
    MemoryBlock(::mikrosimage::alloc::Allocator* const pAllocator, char* pBuffer, const size_t& size);
    MemoryBlock(::mikrosimage::alloc::Allocator* const pAllocator, const size_t size);

    ~MemoryBlock();

    template<typename T>
    T* getPtr();

    template<typename T>
    const T* getPtr() const;

    const size_t size() const;

    bool hold(const char* ptr) const;
};

template<typename T>
T* MemoryBlock::getPtr() {
    return reinterpret_cast<T*> (m_pBuffer);
}

template<typename T>
const T* MemoryBlock::getPtr() const {
    return reinterpret_cast<T*> (m_pBuffer);
}

typedef ::boost::shared_ptr<MemoryBlock> MemoryBlockPtr;


#endif /* MEMORYBLOCK_H_ */
