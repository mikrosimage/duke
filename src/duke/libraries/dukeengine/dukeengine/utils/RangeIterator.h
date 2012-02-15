#ifndef RANGEITERATOR_H_
#define RANGEITERATOR_H_

#include "EPlaybackState.h"
#include <sequence/Range.h>

struct RangeIterator {
    RangeIterator();
    RangeIterator(const sequence::Range &withinRange, bool isCycling, unsigned int startAt, EPlaybackState strategy);
    bool empty()const;
    unsigned int front() const;
    void popFront();
private:
    sequence::Range range;
    unsigned int initialPosition;
    bool isCycling;
    EPlaybackState strategy;
    unsigned int index;
};

#endif /* RANGEITERATOR_H_ */
