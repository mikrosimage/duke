#include "RangeIterator.h"

#include <cassert>
#include <cstdio>
#include <limits.h>

using namespace sequence;

static inline int balance(unsigned int index) {
    const bool isOdd = index & 0x1;
    const unsigned int step = (index >> 1) + isOdd;
    return isOdd ? step : -step;
}

RangeIterator::RangeIterator() :
                range(0, 0), initialPosition(0), isCycling(false), strategy(FORWARD), index(INT_MAX) {
}

RangeIterator::RangeIterator(const Range &withinRange, bool isCycling, unsigned int startAt, EPlaybackState strategy) :
                range(withinRange), initialPosition(startAt), isCycling(isCycling), strategy(strategy), index(0) {
}

static inline int getOffset(unsigned int index, EPlaybackState strategy) {
    assert(index < INT_MAX);
    switch (strategy) {
        case FORWARD:
            return index;
        case BALANCE:
            return balance(index);
        case REVERSE:
            return -index;
        default:
            printf("Unknown value of ERangeIteratorStrategy\n");
            return 0;
    }
}

bool RangeIterator::empty() const {
    return index >= INT_MAX;
}

unsigned int RangeIterator::front() const {
    const int offset = getOffset(index, strategy);
    return (isCycling ? range.offsetLoopFrame(initialPosition, offset) : range.offsetClampFrame(initialPosition, offset)).first;
}

void RangeIterator::popFront() {
    ++index;
}
