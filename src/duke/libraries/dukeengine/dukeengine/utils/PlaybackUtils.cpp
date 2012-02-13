#include "PlaybackUtils.h"

#include <cassert>
#include <cstdio>
#include <limits.h>

namespace sequence {

static inline int balance(unsigned int index) {
    const bool isOdd = index & 0x1;
    const unsigned int step = (index >> 1) + isOdd;
    return isOdd ? step : -step;
}

RangeIterator::RangeIterator(const Range &withinRange, bool isCycling, unsigned int startAt, ERangeIteratorStrategy strategy) :
                range(withinRange), initialPosition(startAt), isCycling(isCycling), strategy(strategy), index(0) {
}

static inline int getOffset(unsigned int index, ERangeIteratorStrategy strategy) const {
    assert(index<INT_MAX);
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

unsigned int RangeIterator::next() {
    const int offset = getOffset(index, strategy);
    ++index;
    return isCycling ? range.offsetLoopFrame(initialPosition, offset) : range.offsetClampFrame(initialPosition, offset);
}

} // namespace sequence
