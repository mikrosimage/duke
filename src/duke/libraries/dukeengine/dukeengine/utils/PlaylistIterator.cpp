/*
 * PlaylistIterator.cpp
 *
 *  Created on: 15 févr. 2012
 *      Author: Guillaume Chatelet
 */

#include "PlaylistIterator.h"

using namespace duke::protocol;

PlaylistIterator::PlaylistIterator() :
                helper(Playlist()), iterator() {
}

PlaylistIterator::PlaylistIterator(const duke::protocol::PlaylistHelper &helper, EPlaybackState state, unsigned int frame, const sequence::Range &overRange) :
                helper(helper), iterator(overRange, helper.playlist.loop(), frame, state) {
}

bool PlaylistIterator::empty() const {
    return frames.empty() && iterator.empty();
}

duke::protocol::MediaFrame PlaylistIterator::front() const {
    assert(!frames.empty());
    return frames[0];
}

void PlaylistIterator::popFront() {
    assert(!frames.empty());
    if (!frames.empty()) {
        frames.erase(frames.begin());
        return;
    }
    while (frames.empty()) {
        if (iterator.empty())
            return;
        iterator.popFront();
        helper.mediaFramesAt(iterator.front(), frames);
    }
}

