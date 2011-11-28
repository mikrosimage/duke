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

struct UnlimitedForwardRange : public ::OnePassRange<std::ptrdiff_t> {
public:
    typedef std::ptrdiff_t value_type;
private:
    value_type m_Current;
public:
    UnlimitedForwardRange() :
        m_Current(0) {
    }
    virtual bool empty() const {
        return false;
    }
    virtual void popFront() {
        ++m_Current;
    }
    virtual value_type front() {
        return m_Current;
    }
};

struct BalancingRange : public ::OnePassRange<std::ptrdiff_t> {
public:
    typedef std::ptrdiff_t value_type;
private:
    std::size_t m_Current;

public:
    BalancingRange() :
        m_Current(0) {
    }
    bool empty() const {
        return false;
    }
    void popFront() {
        ++m_Current;
    }
    value_type front() {
        const std::size_t modulo = m_Current & 0x1;
        const std::size_t step = (m_Current >> 1) + modulo;
        return modulo ? step : -step;
    }
};

struct DrivingRange : public ::OnePassRange<std::ptrdiff_t> {
public:
    typedef std::ptrdiff_t value_type;
private:
    int32_t m_Speed; // should be const but would prevent operator=()

    UnlimitedForwardRange m_Forward;
    BalancingRange m_Balancing;

public:
    DrivingRange(const DrivingRange &other) :
        m_Speed(other.m_Speed), m_Forward(other.m_Forward), m_Balancing(other.m_Balancing) {
    }
    DrivingRange(int32_t speed) :
        m_Speed(speed) {
    }
    bool empty() const {
        return false;
    }
    void popFront() {
        if (m_Speed == 0)
            m_Balancing.popFront();
        else
            m_Forward.popFront();
    }
    value_type front() {
        return m_Speed == 0 ? m_Balancing.front() : m_Speed > 0 ? m_Forward.front() : -m_Forward.front();
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
struct OffsetRange : public ::OnePassRange<typename RANGE::value_type> {
    typedef typename RANGE::value_type value_type;
private:
    value_type m_Offset; // should be const but would prevent operator=()
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
    value_type m_LowerBound; // should be const but would prevent operator=()
    value_type m_UpperBound; // should be const but would prevent operator=()
    value_type m_ModuloSize; // should be const but would prevent operator=()
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

struct PlaylistFrameRange : public ModuloIndexRange<OffsetRange<DrivingRange> > {
    typedef OffsetRange<DrivingRange> OFFSET;
    typedef ModuloIndexRange<OFFSET> MODULO;
public:
    static std::ptrdiff_t adjustBound(std::ptrdiff_t keepFrame) {
        if (keepFrame == 0)
            return 0;
        return keepFrame > 0 ? keepFrame + 1 : keepFrame - 1;
    }
    PlaylistFrameRange(std::ptrdiff_t firstFrame, std::ptrdiff_t lastFrame, std::ptrdiff_t readFromFrame, int32_t speed) :
        MODULO(OFFSET(DrivingRange(speed), readFromFrame), firstFrame, lastFrame) {
        assert(firstFrame<=lastFrame);
        if (readFromFrame < firstFrame || readFromFrame > lastFrame)
            throw std::runtime_error("the cursor must be between firstFrame and lastFrame");
    }
};

struct LimitedPlaylistFrameRange : public Limiter<PlaylistFrameRange> {
    typedef Limiter<PlaylistFrameRange> LIMITED;
    LimitedPlaylistFrameRange(std::ptrdiff_t firstFrame, std::ptrdiff_t lastFrame, std::ptrdiff_t readFromFrame, int32_t speed) :
        LIMITED(PlaylistFrameRange(firstFrame, lastFrame, readFromFrame, speed), lastFrame - firstFrame + 1) {
        assert(firstFrame<=lastFrame);
    }

    static LimitedPlaylistFrameRange EMPTY() {
        LimitedPlaylistFrameRange tmp(0, 0, 0, 0);
        tmp.popFront();
        return tmp;
    }
};

} // namespace range

#endif /* PLAYLISTRANGE_H_ */
