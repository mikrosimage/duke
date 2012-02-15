/*
 * PlaylistIterator.h
 *
 *  Created on: 15 févr. 2012
 *      Author: Guillaume Chatelet
 */

#ifndef PLAYLISTITERATOR_H_
#define PLAYLISTITERATOR_H_

#include "EPlaybackState.h"
#include "RangeIterator.h"

#include <dukeapi/sequence/PlaylistHelper.h>

struct PlaylistIterator {
    PlaylistIterator();
    PlaylistIterator(const duke::protocol::PlaylistHelper &helper, EPlaybackState state, unsigned int frame, const sequence::Range &overRange);
    bool empty()const;
    duke::protocol::MediaFrame front() const;
    void popFront();
private:
    duke::protocol::PlaylistHelper helper;
    duke::protocol::MediaFrames frames;
    RangeIterator iterator;
};
#endif /* PLAYLISTITERATOR_H_ */
