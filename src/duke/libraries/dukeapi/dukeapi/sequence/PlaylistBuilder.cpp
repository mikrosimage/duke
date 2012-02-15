/*
 * PlaylistBuilder.cpp
 *
 *  Created on: 7 févr. 2012
 *      Author: Guillaume Chatelet
 */

#include <sequence/Range.h>
#include "PlaylistBuilder.h"

using namespace std;
using namespace sequence;

namespace duke {
namespace protocol {

static inline void set(FrameRange* pRange, const Range &range) {
    pRange->set_first(range.first);
    pRange->set_last(range.last);
}

TrackBuilder::TrackBuilder(Track &track, const char *name) :
                track(track) {
    track.set_name(name);
}

Media& TrackBuilder::addImage(const char *filename, const Range &record) {
    Clip *pClip = track.add_clip();
    set(pClip->mutable_record(), record);
    Media &media = *pClip->mutable_media();
    media.set_type(Media_Type_SINGLE_IMAGE);
    media.set_filename(filename);
    return media;
}

Media& TrackBuilder::addMedia(const char *filename, const Range&record, const Range &source, const Media_Type mediaType) {
    Clip *pClip = track.add_clip();
    set(pClip->mutable_record(), record);
    Media &media = *pClip->mutable_media();
    media.set_type(mediaType);
    media.set_filename(filename);
    set(media.mutable_source(), source);
    return media;
}

Media& TrackBuilder::addMedia(const char *filename, const Range &record, const uint32_t offset, const Media_Type mediaType) {
    return addMedia(filename, record, Range(record.first + offset, record.last + offset), mediaType);
}

TrackBuilder PlaylistBuilder::addTrack(const char *trackName) {
    return TrackBuilder(*playlist.add_track(), trackName);
}

PlaylistBuilder::operator ::duke::protocol::Playlist() {
    return playlist;
}

} // namespace protocol
} /* namespace duke */
