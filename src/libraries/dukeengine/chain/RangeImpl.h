#ifndef SIMPLEITERATORS_H_
#define SIMPLEITERATORS_H_

#include "Range.h"
#include <cassert>
#include <cstdint>

/**
 * Double ended range
 */
template<typename T = uint64_t>
struct SimpleIndexRange : public DoubleEndedRange<T> {
    typedef T value_type;
private:
    typedef DoubleEndedRange<T> RANGE;
    T begin;
    T pastEnd;
public:
    SimpleIndexRange(T begin, T end) :
        begin(begin), pastEnd(end) {
        assert(begin <= pastEnd);
    }
    virtual ~SimpleIndexRange() {
    }
    virtual bool empty() const {
        assert(begin <= pastEnd);
        return begin == pastEnd;
    }
    virtual void popFront() {
        if (!empty())
            ++begin;
    }
    virtual T front() {
        assert(!empty());
        return begin;
    }
    virtual RANGE* save() const {
        return new SimpleIndexRange(*this);
    }
    virtual T back() {
        assert(!empty());
        return pastEnd - 1;
    }
    virtual void popBack() {
        if (!empty())
            --pastEnd;
    }
};

template<typename T = uint64_t>
struct ConstantIndexRange : public ForwardRange<T> {
    typedef T value_type;
private:
    typedef ForwardRange<T> RANGE;
    std::size_t left;
    T value;

public:
    ConstantIndexRange(const T constant, std::size_t size) :
        left(size), value(constant) {
    }
    virtual ~ConstantIndexRange() {
    }
    bool empty() const {
        return left == 0;
    }
    void popFront() {
        if (!empty())
            --left;
    }
    T front() {
        assert(!empty());
        return value;
    }
    RANGE* save() const {
        return new ConstantIndexRange(*this);
    }
};

struct BalancingIndexRange : public ForwardRange<std::ptrdiff_t> {
private:
    typedef ForwardRange<std::ptrdiff_t> RANGE;
    const std::size_t m_EndIndex;
    const std::size_t m_InternalBound;
    std::size_t m_Index;

public:
    explicit BalancingIndexRange(const std::size_t index, const std::size_t endIndex, const std::ptrdiff_t bound) :
        m_EndIndex(endIndex), m_InternalBound(bound == 0 ? -1 : bound > 0 ? 2 * (bound - 1) : -(2 * bound + 1)), m_Index(index) {
        assert(m_Index <= m_EndIndex);
    }
    virtual ~BalancingIndexRange() {
    }
    bool empty() const {
        assert(m_Index <= m_EndIndex);
        return m_Index == m_EndIndex;
    }
    void popFront() {
        if (!empty())
            ++m_Index;
    }
    std::ptrdiff_t front() {
        assert(!empty());
        const std::size_t virtualIndex = m_Index < m_InternalBound ? m_Index : m_InternalBound + 2 * (m_Index - m_InternalBound);
        const std::size_t modulo = virtualIndex & 0x1;
        const std::size_t step = (virtualIndex >> 1) + modulo;
        assert( step <= std::size_t((void*)(-1)) );
        const std::ptrdiff_t value = modulo ? step : -step;
        return value;
    }
    RANGE* save() const {
        return new BalancingIndexRange(*this);
    }
};

template<typename T = int64_t>
struct Negater : public ForwardRange<T> {
private:
    typedef ForwardRange<T> RANGE;
    std::auto_ptr<RANGE> m_pDelegateRange;

public:
    explicit Negater(const Negater& other) :
        m_pDelegateRange(other.m_pDelegateRange->save()) {
    }
    explicit Negater(const RANGE &rangeToConsume) :
        m_pDelegateRange(rangeToConsume.save()) {
    }
    virtual ~Negater() {
    }
    bool empty() const {
        return m_pDelegateRange->empty();
    }
    void popFront() {
        m_pDelegateRange->popFront();
    }
    T front() {
        return -m_pDelegateRange->front();
    }
    RANGE* save() const {
        return new Negater(*this);
    }
};

template<typename T = uint64_t>
struct RetroRange : public DoubleEndedRange<T> {
private:
    typedef DoubleEndedRange<T> RANGE;
    std::auto_ptr<RANGE> m_pDelegateRange;

public:
    explicit RetroRange(const RetroRange& other) :
        m_pDelegateRange(other.m_pDelegateRange->save()) {
    }
    explicit RetroRange(const RANGE &rangeToConsume) :
        m_pDelegateRange(rangeToConsume.save()) {
    }
    virtual ~RetroRange() {
    }
    bool empty() const {
        return m_pDelegateRange->empty();
    }
    void popFront() {
        m_pDelegateRange->popBack();
    }
    T front() {
        return m_pDelegateRange->back();
    }
    T back() {
        return m_pDelegateRange->front();
    }
    void popBack() {
        m_pDelegateRange->popFront();
    }
    RANGE* save() const {
        return new RetroRange(*this);
    }
};

template<typename T = uint64_t>
struct OffsetRange : public ForwardRange<T> {
private:
    typedef ForwardRange<T> RANGE;
    const T m_Offset;
    std::auto_ptr<RANGE> m_pDelegateRange;

public:
    explicit OffsetRange(const OffsetRange& other) :
        m_Offset(other.m_Offset), m_pDelegateRange(other.m_pDelegateRange->save()) {
    }
    explicit OffsetRange(const RANGE &rangeToConsume, const T offset) :
        m_Offset(offset), m_pDelegateRange(rangeToConsume.save()) {
    }
    virtual ~OffsetRange() {
    }
    bool empty() const {
        return m_pDelegateRange->empty();
    }
    void popFront() {
        m_pDelegateRange->popFront();
    }
    T front() {
        return m_pDelegateRange->front() + m_Offset;
    }
    RANGE* save() const {
        return new OffsetRange(*this);
    }
};

template<typename T = uint64_t>
struct ModuloIndexRange : public ForwardRange<T> {
private:
    typedef ForwardRange<T> RANGE;
    const T m_LowerBound;
    const T m_UpperBound;
    const T m_ModuloSize;
    std::auto_ptr<RANGE> m_pDelegateRange;

public:
    ModuloIndexRange(const ModuloIndexRange &other) :
        m_LowerBound(other.m_LowerBound), m_UpperBound(other.m_UpperBound), m_ModuloSize(other.m_ModuloSize), m_pDelegateRange(other.m_pDelegateRange->save()) {
    }
    explicit ModuloIndexRange(const RANGE &rangeToConsume, const T lowerBound, const T upperBound) :
        m_LowerBound(lowerBound), m_UpperBound(upperBound), m_ModuloSize(upperBound - m_LowerBound + 1), m_pDelegateRange(rangeToConsume.save()) {
        assert(m_UpperBound>=m_LowerBound);
        assert(m_ModuloSize>0);
        assert(m_pDelegateRange.get());
    }
    virtual ~ModuloIndexRange() {
    }
    bool empty() const {
        return m_pDelegateRange->empty();
    }
    void popFront() {
        assert(!empty());
        m_pDelegateRange->popFront();
    }
    T front() {
        assert(!empty());
        T value = m_pDelegateRange->front();

        while (value < m_LowerBound)
            value += m_ModuloSize;
        while (value > m_UpperBound)
            value -= m_ModuloSize;
        return value;
    }
    RANGE* save() const {
        return new ModuloIndexRange(*this);
    }
};

#include <cstdlib>
struct RandomIterator : public OnePassRange<std::size_t> {
private:
    std::size_t left;
    std::size_t current;
    void generate() {
        current = std::rand();
    }
public:
    RandomIterator() :
        left(0) {
        std::srand(0);
    }
    RandomIterator(std::size_t size) :
        left(size) {
        std::srand(0);
        generate();
    }
    virtual ~RandomIterator() {
    }
    bool empty() const {
        return left == 0;
    }
    void popFront() {
        assert(!empty());
        generate();
        --left;
    }
    std::size_t front() {
        assert(!empty());
        return current;
    }
};

ForwardRange<std::ptrdiff_t>* getPlaylistRange(std::ptrdiff_t playlistSize, std::ptrdiff_t readFrom, std::ptrdiff_t balancingBound, bool isReverse);

#endif /* SIMPLEITERATORS_H_ */
