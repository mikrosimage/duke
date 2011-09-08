/*
 * MemoryBlock.h
 *
 *  Created on: 19 avr. 2010
 *      Author: Guillaume Chatelet
 */

#include "MemoryBlock.h"
#include "Allocators.h"

MemoryBlock::MemoryBlock(::mikrosimage::alloc::Allocator* const pAllocator, char* pBuffer, const size_t& size) :
    m_pAllocator(pAllocator), m_uSize(pBuffer == NULL ? 0 : size), m_pBuffer(pBuffer) {
}

MemoryBlock::MemoryBlock(::mikrosimage::alloc::Allocator* const pAllocator, const size_t size) :
    m_pAllocator(pAllocator), m_uSize(size), m_pBuffer(m_pAllocator->malloc(m_uSize)) {
    assert( m_pAllocator != NULL );
    if (m_pBuffer == NULL)
        throw std::bad_alloc();
}



//MemoryBlock::MemoryBlock(::mikrosimage::alloc::Allocator* const pAllocator, boost::iostreams::mapped_file_params & params) :
//    m_pAllocator(pAllocator) {
//    assert( m_pAllocator != NULL );
//
//    mf.open(params);
//    if(!mf.is_open())
//        throw std::bad_alloc();
//
//    m_uSize = mf.size();
//    m_pBuffer = m_pAllocator->malloc(m_uSize);
//    memcpy(m_pBuffer, mf.const_data(), m_uSize);
//    mf.close();
//    if (m_pBuffer == NULL)
//        throw std::bad_alloc();
//}

MemoryBlock::~MemoryBlock() {
    m_pAllocator->free(m_pBuffer);
}

const size_t MemoryBlock::size() const {
    return m_uSize;
}

bool MemoryBlock::hold(const char* ptr) const {
    return (ptr >= m_pBuffer) && (ptr <= (m_pBuffer + m_uSize));
}
