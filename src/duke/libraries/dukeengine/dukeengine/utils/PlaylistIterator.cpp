/*
 * PlaylistIterator.cpp
 *
 *  Created on: 15 févr. 2012
 *      Author: Guillaume Chatelet
 */

#include "PlaylistIterator.h"

#include <cstdio>

using namespace duke::protocol;

PlaylistIterator::PlaylistIterator() :
                helper(Playlist()), iterator() {
}

PlaylistIterator::PlaylistIterator(const duke::protocol::PlaylistHelper &helper, EPlaybackState state, unsigned int frame, const sequence::Range &overRange) :
                helper(helper), iterator(overRange, frame, state) {
    populate();
}

bool PlaylistIterator::empty() const {
    return frames.empty() && iterator.empty();
}

duke::protocol::MediaFrame PlaylistIterator::front() const {
    if(frames.empty())
        printf("very bad");
    assert(!frames.empty());
    return frames[0];
}

void PlaylistIterator::popFront() {
    if (!frames.empty()) {
        frames.erase(frames.begin());
        return;
    }
    populate();
}

void PlaylistIterator::populate() {
    while (frames.empty()) {
        if (iterator.empty())
            return;
        helper.mediaFramesAt(iterator.front(), frames);
        iterator.popFront();
    }
}
