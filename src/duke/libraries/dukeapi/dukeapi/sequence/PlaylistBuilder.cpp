/*
 * PlaylistBuilder.cpp
 *
 *  Created on: 7 févr. 2012
 *      Author: Guillaume Chatelet
 */

#include "PlaylistBuilder.h"

using namespace std;

namespace duke {
namespace protocol {

namespace range {
FrameRange make(uint32_t first, uint32_t last) {
    FrameRange range;
    range.set_first(first);
    range.set_last(last);
    return range;
}
}  // namespace range

TrackBuilder::TrackBuilder(Track &track, const char *name) :
                track(track) {
    track.set_name(name);
}

Media& TrackBuilder::addImage(const char *filename, const FrameRange record) {
    Clip *pClip = track.add_clip();
    pClip->mutable_record()->CopyFrom(record);
    Media &media = *pClip->mutable_media();
    media.set_type(Media_Type_SINGLE_IMAGE);
    media.set_filename(filename);
    return media;
}

Media& TrackBuilder::addMedia(const char *filename, const FrameRange record, const FrameRange source, const Media_Type mediaType) {
    Clip *pClip = track.add_clip();
    pClip->mutable_record()->CopyFrom(record);
    Media &media = *pClip->mutable_media();
    media.set_type(mediaType);
    media.set_filename(filename);
    media.mutable_source()->CopyFrom(source);
    return media;
}

Media& TrackBuilder::addMedia(const char *filename, const FrameRange record, const uint32_t offset, const Media_Type mediaType) {
    return addMedia(filename, record, range::make(record.first() + offset, record.last() + offset), mediaType);
}

TrackBuilder PlaylistBuilder::addTrack(const char *trackName) {
    return TrackBuilder(*playlist.add_track(), trackName);
}

PlaylistBuilder::operator ::duke::protocol::Playlist() {
    return playlist;
}

} // namespace protocol
} /* namespace duke */
