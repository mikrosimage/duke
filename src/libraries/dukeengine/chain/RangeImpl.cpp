/*
 * RangeImpl.cpp
 *
 *  Created on: 15 nov. 2010
 *      Author: Guillaume Chatelet
 */

#include "RangeImpl.h"
#include <stdexcept>

using namespace std;

ForwardRange<ptrdiff_t>* getPlaylistRange(ptrdiff_t playlistSize, ptrdiff_t readFrom, ptrdiff_t balancingBound, bool isReverse) {
    if (!(playlistSize > 0))
        throw runtime_error("playlist size must be >0");
    if (readFrom < 0 || readFrom >= playlistSize)
        throw runtime_error("the cursor must be between 0 and playlist size");
    if (abs(balancingBound) >= playlistSize)
        throw runtime_error("balancing bound must be within playlist size");

    BalancingIndexRange forwardRange(0, playlistSize, balancingBound);
    Negater<ptrdiff_t> negatedRange(forwardRange);
    ForwardRange<ptrdiff_t> *pDrivingRange = NULL;
    if (isReverse)
        pDrivingRange = &negatedRange;
    else
        pDrivingRange = &forwardRange;

    OffsetRange<ptrdiff_t> offsetRange( *pDrivingRange, readFrom);
    return new ModuloIndexRange<ptrdiff_t> ( offsetRange , 0, playlistSize - 1);
}
