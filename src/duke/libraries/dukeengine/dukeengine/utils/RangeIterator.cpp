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
                range(0, 0), initialPosition(0), strategy(FORWARD), index(INT_MAX), limit(INT_MAX) {
}

RangeIterator::RangeIterator(const Range &withinRange, unsigned int startAt, EPlaybackState strategy) :
                range(withinRange), initialPosition(startAt), strategy(strategy), index(0), limit(range.duration()) {
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
    return index >= limit;
}

unsigned int RangeIterator::front() {
    const int offset = getOffset(index, strategy);
    return range.offsetLoopFrame(initialPosition, offset).first;
}

void RangeIterator::popFront() {
    ++index;
}
