#ifndef RANGE_H_
#define RANGE_H_

/**
 * Based on the work of Andrei Alexandrescu about Range based algorithm
 * http://www.informit.com/articles/printerfriendly.aspx?p=1407357
 */

#include <cstddef>
#include <memory>

/**
 * allow simple traversal
 */
template<typename T>
struct OnePassRange {
    virtual ~OnePassRange() {
    }
    virtual bool empty() const = 0;
    virtual void popFront() = 0;
    virtual T front() = 0;
};

/**
 * simple traversal + memoization
 */
template<typename T>
struct ForwardRange : public OnePassRange<T> {
    virtual ~ForwardRange() {
    }
    virtual ForwardRange* save() const = 0;
};

/**
 * bidirectionnal traversal + memoization
 */
template<typename T>
struct DoubleEndedRange : public ForwardRange<T> {
    virtual ~DoubleEndedRange() {
    }
    virtual T back() = 0;
    virtual void popBack() = 0;
    virtual DoubleEndedRange* save() const = 0;
};

/**
 * simple traversal + memoization + infinite range
 */
template<typename T>
struct RandomAccessInfiniteRange : ForwardRange<T> {
    virtual ~RandomAccessInfiniteRange() {
    }
    virtual T at(std::size_t index) = 0;
    virtual RandomAccessInfiniteRange slice(std::size_t fromIndex, std::size_t toIndex) const = 0;
};

/**
 * bidirectionnal + memoization + finite range
 */
template<typename T>
struct RandomAccessFiniteRange : DoubleEndedRange<T> {
    virtual ~RandomAccessFiniteRange() {
    }
    virtual T at(std::size_t index) = 0;
    virtual RandomAccessFiniteRange slice(std::size_t fromIndex, std::size_t toIndex) const = 0;
};

#endif /* RANGE_H_ */
