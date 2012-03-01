/*
 * PlaylistIterator.h
 *
 *  Created on: 15 fevr. 2012
 *      Author: Guillaume Chatelet
 */

#ifndef CACHEITERATOR_H_
#define CACHEITERATOR_H_

#include "EPlaybackState.h"
#include "RangeIterator.h"

#include <dukeapi/sequence/PlaylistHelper.h>

struct CacheIterator {
    CacheIterator();
    CacheIterator(const duke::protocol::PlaylistHelper &helper, EPlaybackState state, unsigned int frame, const sequence::Range &overRange);
    bool empty()const;
    duke::protocol::MediaFrame front() const;
    void popFront();
private:
    void populate();
    duke::protocol::PlaylistHelper helper;
    duke::protocol::MediaFrames frames;
    RangeIterator iterator;
};
#endif /* CACHEITERATOR_H_ */
