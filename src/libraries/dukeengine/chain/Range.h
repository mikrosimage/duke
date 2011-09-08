/*
 * OnePassRange.h
 *
 *  Created on: 10 nov. 2010
 *      Author: Guillaume Chatelet
 */

#ifndef RANGE_H_
#define RANGE_H_

#include <cstddef>
#include <memory>

template<typename T>
struct OnePassRange {
    virtual ~OnePassRange() {
    }
    virtual bool empty() const = 0;
    virtual void popFront() = 0;
    virtual T front() = 0;
};

template<typename T>
struct ForwardRange : public OnePassRange<T> {
    virtual ForwardRange* save() const = 0;
};

template<typename T>
struct DoubleEndedRange : public ForwardRange<T> {
    virtual T back() = 0;
    virtual void popBack() = 0;
    virtual DoubleEndedRange* save() const = 0;
};

template<typename T>
struct RandomAccessInfiniteRange : ForwardRange<T> {
    virtual T at(std::size_t index) = 0;
    virtual RandomAccessInfiniteRange slice(std::size_t fromIndex, std::size_t toIndex) const = 0;
};

template<typename T>
struct RandomAccessFiniteRange : DoubleEndedRange<T> {
    virtual T at(std::size_t index) = 0;
    virtual RandomAccessFiniteRange slice(std::size_t fromIndex, std::size_t toIndex) const = 0;
};

#endif /* RANGE_H_ */
