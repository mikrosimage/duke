/*
 * PlaylistHelper.cpp
 *
 *  Created on: 8 f�vr. 2012
 *      Author: Guillaume Chatelet
 */

#include "PlaylistHelper.h"
#include <sequence/Range.h>
#include <sequence/DisplayUtils.h>
#include <sequence/BrowseItem.h>

#include <boost/functional/hash.hpp>

#include <iostream>
#include <algorithm>
#include <sstream>
#include <climits>

using namespace std;
using namespace sequence;

namespace duke {
namespace protocol {

string MediaFrame::filename() const {
    boost::filesystem::path path;
    switch (type) {
        case duke::protocol::Media_Type_SINGLE_IMAGE:
            path = item.path;
            break;
        case duke::protocol::Media_Type_IMAGE_SEQUENCE:
            path = item.path / sequence::instanciatePattern(item.sequence.pattern, source);
            break;
        default:
            cerr << "can't decode movies for the moment" << endl;
            break;
    }
    return path.make_preferred().string();
}

template<class InputIterator, class Function>
Function for_each_adjacent(InputIterator first, InputIterator last, Function f) {
    typedef typename InputIterator::value_type value_type;
    value_type previous;
    bool firstIteration = true;
    for (; first != last; ++first) {
        if (!firstIteration)
            f(previous, *first);
        previous = *first;
        firstIteration = false;
    }
    return f;
}

struct MinMaxRange {
    MinMaxRange() :
                    empty(true) {
    }
    void operator()(const Range &range) {
        if (empty)
            current = range;
        else
            current = Range(min(current.first, range.first), max(current.last, range.last));
        empty = false;
    }
    operator Range() const {
        return current;
    }
private:
    bool empty;
    Range current;
};

static inline bool operator<(const Range &a, const Range &b) {
    return a.last < b.first;
}

static inline void assertBefore(const Range &first, const Range &second) {
    if (!(first < second)) {
        ostringstream ss;
        ss << "Ranges overlap or are not ordered : " << first << ' ' << second;
        throw PlaylistError(ss.str());
    }
}

static inline void checkMedia(const Media &media) {
    if (media.filename().empty())
        throw PlaylistError(string("Media must have a filename\n") + media.DebugString());
    if (media.type() == Media_Type_IMAGE_SEQUENCE && media.filename().find('#') == string::npos)
        throw PlaylistError(string("Sequence filename must have a '#' in it : ") + media.filename());
}

static inline void checkClip(const Clip &clip) {
    if (clip.has_media())
        checkMedia(clip.media());
}

static inline Range make(const FrameRange& range) {
    return sequence::Range(range.first(), range.last());
}

static sequence::BrowseItem prepare(const Clip &clip) {
    if (!clip.has_media())
        return sequence::BrowseItem();
    const Media &media = clip.media();
    switch (media.type()) {
        case Media_Type_MOVIE_CONTAINER:
        case Media_Type_SINGLE_IMAGE:
            return sequence::create_file(media.filename());
        case Media_Type_IMAGE_SEQUENCE: {
            boost::filesystem::path path(media.filename());
            return sequence::create_sequence(path.parent_path(), sequence::parsePattern(path.filename().string()), make(media.source()));
        }
        default:
            return sequence::BrowseItem();
    }
}

TrackHelper::TrackHelper(const Track &track) : track(track) {
    recRanges.reserve(track.clip_size());
    for (int i = 0; i < track.clip_size(); ++i) {
        const Clip &clip = track.clip(i);
        recRanges.push_back(make(clip.record()));
        items.push_back(prepare(clip));
    }
    for_each(track.clip().begin(), track.clip().end(), checkClip);
    for_each_adjacent(recRanges.begin(), recRanges.end(), assertBefore);
    range = for_each(recRanges.begin(), recRanges.end(), MinMaxRange());
}

unsigned int TrackHelper::index(const unsigned int frame) const {
    unsigned int sum = 0;
    RangeCItr itr = recRanges.begin();
    for (; itr != recRanges.end(); ++itr) {
        if (frame < itr->first)
            return sum;
        if (itr->contains(frame))
            return sum + (frame - itr->first);
        sum += itr->duration();
    }
    return sum;
}

const RangeCItr TrackHelper::rangeContaining(const unsigned int frame) const {
    const RangeCItr end = recRanges.end();
    const RangeCItr lower = lower_bound(recRanges.begin(), end, Range(frame, frame), &operator<);
    if (lower != end && lower->first > frame)
        return end;
    return lower;
}

const unsigned int TrackHelper::clipIndexContaining(const unsigned int frame) const {
    const RangeCItr end = recRanges.end();
    const RangeCItr found = rangeContaining(frame);
    if (found == end)
        return UINT_MAX;
    return distance(recRanges.begin(), found);
}

const bool TrackHelper::contains(const unsigned int frame) const {
    return rangeContaining(frame) != recRanges.end();
}

PlaylistHelper::PlaylistHelper() {
}

struct ClipGatherer {
    ClipGatherer(Ranges&ranges) : ranges(ranges){}
    void operator()(const TrackHelper& helper){
        copy(helper.recRanges.begin(), helper.recRanges.end(), back_inserter(ranges));
    }
    Ranges &ranges;
};

static inline bool rangeLess(const Range &a, const Range &b) {
    return a.first == b.first ? a.last < b.last : a.first < b.first;
}

PlaylistHelper::PlaylistHelper(const Scene &_scene) :
                scene(_scene) {
    Ranges trackRanges;
    for (int i = 0; i < scene.track_size(); ++i) {
        tracks.push_back(TrackHelper(scene.track(i)));
        trackRanges.push_back(tracks.back().range);
    }
    range = for_each(trackRanges.begin(), trackRanges.end(), MinMaxRange());
    for_each(tracks.begin(), tracks.end(), ClipGatherer(allClips));
    sort(allClips.begin(), allClips.end(), &rangeLess);
}

bool PlaylistHelper::empty() const{
    unsigned int sum = 0;
    for (vector<TrackHelper>::const_iterator itr = tracks.begin(); itr != tracks.end(); ++itr)
        sum += itr->items.size();
    return sum==0;
}

unsigned int PlaylistHelper::index(const unsigned int frame) const {
    unsigned int sum = 0;
    for (vector<TrackHelper>::const_iterator itr = tracks.begin(); itr != tracks.end(); ++itr)
        sum += itr->index(frame);
    return sum;
}

uint16_t PlaylistHelper::tracksAt(const unsigned int frame) const {
    uint16_t sum = 0;
    for (vector<TrackHelper>::const_iterator itr = tracks.begin(); itr != tracks.end(); ++itr)
        sum += itr->contains(frame);
    return sum;
}

void PlaylistHelper::mediaFramesAt(const unsigned int frame, MediaFrames &frames) const {
    frames.clear();
    uint16_t track = 0;
    for (vector<TrackHelper>::const_iterator itr = tracks.begin(); itr != tracks.end(); ++itr, ++track) {
        const unsigned int clipIndex = itr->clipIndexContaining(frame);
        if (clipIndex == UINT_MAX)
            continue;
        const Clip &clip = itr->track.clip(clipIndex);
        // media dependent
        unsigned int sourceFrame = 0;
        if (clip.has_media()) {
            const bool reverse = clip.media().reverse();
            const sequence::Range source = make(clip.media().source());
            sourceFrame = sequence::interpolateSource(frame, source, make(clip.record()), reverse);
        }
        frames.push_back(MediaFrame( //
                        PlaylistIndex(frame, track), //
                        sourceFrame, //
                        clip.media().type(), //
                        itr->items[clipIndex]));
    }
}

void PlaylistHelper::clipsAt(const unsigned int frame, Clips &clips) const {
    clips.clear();
    for (vector<TrackHelper>::const_iterator itr = tracks.begin(); itr != tracks.end(); ++itr) {
        const unsigned int clipIndex = itr->clipIndexContaining(frame);
        if (clipIndex == UINT_MAX)
            continue;
        const Clip &clip = itr->track.clip(clipIndex);
        clips.push_back(clip);
    }
}

} /* namespace protocol */
} /* namespace duke */
