/*
 * CacheIterator.cpp
 *
 *  Created on: 15 fevr. 2012
 *      Author: Guillaume Chatelet
 */

#include "CacheIterator.h"

#include <cstdio>

using namespace duke::protocol;

CacheIterator::CacheIterator() :
                helper(Scene()), iterator() {
}

CacheIterator::CacheIterator(const duke::protocol::PlaylistHelper &helper, EPlaybackState state, unsigned int frame, const sequence::Range &overRange) :
                helper(helper), iterator(overRange, frame, state) {
    assert(overRange.contains(frame));
    populate();
}

bool CacheIterator::empty() const {
    return frames.empty() && iterator.empty();
}

duke::protocol::MediaFrame CacheIterator::front() const {
    if (frames.empty())
        throw std::runtime_error("Invalid state");
    assert(!frames.empty());
    return frames[0];
}

void CacheIterator::popFront() {
    if (!frames.empty())
        frames.erase(frames.begin());
    populate();
}

void CacheIterator::populate() {
    while (frames.empty()) {
        if (iterator.empty())
            return;
        helper.mediaFramesAt(iterator.front(), frames);
        iterator.popFront();
    }
}
