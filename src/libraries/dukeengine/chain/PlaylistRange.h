/*
 * PlaylistRange.h
 *
 *  Created on: 3 nov. 2011
 *      Author: Guillaume Chatelet
 */

#ifndef PLAYLISTRANGE_H_
#define PLAYLISTRANGE_H_

#include "Range.h"

#include <stdexcept>
#include <cstddef>
#include <cassert>
#include <cmath>

#include <iostream>

namespace range {

struct SimpleIndexRange : public ::OnePassRange<std::ptrdiff_t> {
public:
    typedef std::ptrdiff_t value_type;
private:
    value_type begin;
    value_type pastEnd;
public:
    SimpleIndexRange(value_type begin, value_type end) :
        begin(begin), pastEnd(end) {
        assert(begin <= pastEnd);
    }
    virtual bool empty() const {
        assert(begin <= pastEnd);
        return begin == pastEnd;
    }
    virtual void popFront() {
        if (!empty())
            ++begin;
    }
    virtual value_type front() {
        assert(!empty());
        return begin;
    }
    virtual value_type back() {
        assert(!empty());
        return pastEnd - 1;
    }
    virtual void popBack() {
        if (!empty())
            --pastEnd;
    }
};

struct BalancingIndexRange : public ::OnePassRange<std::ptrdiff_t> {
public:
    typedef std::ptrdiff_t value_type;
private:
    const std::size_t m_InternalBound;
    std::size_t m_Index;

public:
    explicit BalancingIndexRange(const value_type bound) :
        m_InternalBound(bound == 0 ? -1 : bound > 0 ? 2 * (bound - 1) : -(2 * bound + 1)), m_Index(0) {
    }
    bool empty() const {
        return false;
    }
    void popFront() {
        if (!empty())
            ++m_Index;
    }
    value_type front() {
        const std::size_t virtualIndex = m_Index < m_InternalBound ? m_Index : m_InternalBound + 2 * (m_Index - m_InternalBound);
        const std::size_t modulo = virtualIndex & 0x1;
        const std::size_t step = (virtualIndex >> 1) + modulo;
        assert(step <= std::size_t((void*) (-1)));
        return modulo ? step : -step;
    }
};

template<typename RANGE>
struct Limiter : public ::OnePassRange<typename RANGE::value_type> {
    typedef typename RANGE::value_type value_type;
private:
    RANGE m_Delegate;
    std::size_t m_CurrentCount;
public:
    explicit Limiter(const Limiter& other) :
        m_Delegate(other.m_Delegate), m_CurrentCount(other.m_CurrentCount) {
    }
    explicit Limiter(const RANGE &rangeToConsume, std::size_t countLimit) :
        m_Delegate(rangeToConsume), m_CurrentCount(countLimit) {
    }
    bool empty() const {
        return m_CurrentCount == 0 || m_Delegate.empty();
    }
    void popFront() {
        assert(!empty());
        m_Delegate.popFront();
        --m_CurrentCount;
    }
    value_type front() {
        assert(!empty());
        return m_Delegate.front();
    }
};

template<typename RANGE>
struct Negater : public ::OnePassRange<typename RANGE::value_type> {
    typedef typename RANGE::value_type value_type;
private:
    RANGE m_Delegate;
    const bool m_Negate;
public:
    explicit Negater(const Negater& other) :
        m_Delegate(other.m_Delegate), m_Negate(other.m_Negate) {
    }
    explicit Negater(const RANGE &rangeToConsume, bool doNegate = true) :
        m_Delegate(rangeToConsume), m_Negate(doNegate) {
    }
    bool empty() const {
        return m_Delegate.empty();
    }
    void popFront() {
        m_Delegate.popFront();
    }
    value_type front() {
        return m_Negate ? -m_Delegate.front() : m_Delegate.front();
    }
};

template<typename RANGE>
struct OffsetRange : public ::OnePassRange<typename RANGE::value_type> {
    typedef typename RANGE::value_type value_type;
private:
    const value_type m_Offset;
    RANGE m_Delegate;

public:
    explicit OffsetRange(const OffsetRange& other) :
        m_Offset(other.m_Offset), m_Delegate(other.m_Delegate) {
    }
    explicit OffsetRange(const RANGE &rangeToConsume, const value_type offset) :
        m_Offset(offset), m_Delegate(rangeToConsume) {
    }
    bool empty() const {
        return m_Delegate.empty();
    }
    void popFront() {
        m_Delegate.popFront();
    }
    value_type front() {
        return m_Delegate.front() + m_Offset;
    }
};

template<typename RANGE>
struct ModuloIndexRange : public ::OnePassRange<typename RANGE::value_type> {
    typedef typename RANGE::value_type value_type;
private:
    const value_type m_LowerBound;
    const value_type m_UpperBound;
    const value_type m_ModuloSize;
    RANGE m_Delegate;

public:
    ModuloIndexRange(const ModuloIndexRange &other) :
        m_LowerBound(other.m_LowerBound), m_UpperBound(other.m_UpperBound), m_ModuloSize(other.m_ModuloSize), m_Delegate(other.m_Delegate) {
    }
    explicit ModuloIndexRange(const RANGE &rangeToConsume, const value_type lowerBound, const value_type upperBound) :
        m_LowerBound(lowerBound), m_UpperBound(upperBound), m_ModuloSize(upperBound - m_LowerBound + 1), m_Delegate(rangeToConsume) {
        assert(m_UpperBound>=m_LowerBound);
        assert(m_ModuloSize>0);
    }
    bool empty() const {
        return m_Delegate.empty();
    }
    void popFront() {
        assert(!empty());
        m_Delegate.popFront();
    }
    value_type front() {
        assert(!empty());
        value_type value = m_Delegate.front();

        while (value < m_LowerBound)
            value += m_ModuloSize;
        while (value > m_UpperBound)
            value -= m_ModuloSize;
        return value;
    }
};

struct PlaylistFrameRange : public ModuloIndexRange<OffsetRange<Negater<BalancingIndexRange> > > {
    typedef Negater<BalancingIndexRange> NEGATER;
    typedef OffsetRange<NEGATER> OFFSET;
    typedef ModuloIndexRange<OFFSET> MODULO;
public:
    static ptrdiff_t adjustBound(ptrdiff_t keepFrame) {
        if(keepFrame==0)
            return 0;
        return keepFrame>0 ? keepFrame+1 : keepFrame-1;
    }
    PlaylistFrameRange(ptrdiff_t firstFrame, ptrdiff_t lastFrame, ptrdiff_t readFromFrame, ptrdiff_t keepFrame, bool isReverse) :
        MODULO(OFFSET(NEGATER(BalancingIndexRange(adjustBound(keepFrame)), isReverse), readFromFrame), firstFrame, lastFrame) {
        assert(firstFrame<=lastFrame);
        if (lastFrame - firstFrame <= 0)
            throw std::runtime_error("playlist size must be >0");
        if (readFromFrame < firstFrame || readFromFrame > lastFrame)
            throw std::runtime_error("the cursor must be between firstFrame and lastFrame");
        if (std::abs(keepFrame) >= lastFrame - firstFrame)
            throw std::runtime_error("balancing bound must be within playlist size");
    }
};

struct LimitedPlaylistFrameRange : public Limiter<PlaylistFrameRange> {
    typedef Limiter<PlaylistFrameRange> LIMITED;
public:
    LimitedPlaylistFrameRange(ptrdiff_t firstFrame, ptrdiff_t lastFrame, ptrdiff_t readFromFrame, ptrdiff_t balancingBound, bool isReverse) :
        LIMITED(PlaylistFrameRange(firstFrame, lastFrame, readFromFrame, balancingBound, isReverse), lastFrame - firstFrame + 1) {
    }
};

} // namespace range

#endif /* PLAYLISTRANGE_H_ */
