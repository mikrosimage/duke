#ifndef RANGEITERATOR_H_
#define RANGEITERATOR_H_

#include "EPlaybackState.h"
#include <sequence/Range.h>

struct RangeIterator {
    RangeIterator();
    RangeIterator(const sequence::Range &withinRange, unsigned int startAt, EPlaybackState strategy);
    bool empty()const;
    unsigned int front();
    void popFront();
private:
    sequence::Range range;
    unsigned int initialPosition;
    EPlaybackState strategy;
    unsigned int index;
    unsigned int limit;
};

#endif /* RANGEITERATOR_H_ */
