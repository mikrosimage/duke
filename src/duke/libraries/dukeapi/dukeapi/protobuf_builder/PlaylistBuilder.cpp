/*
 * PlaylistBuilder.cpp
 *
 *  Created on: 7 févr. 2012
 *      Author: Guillaume Chatelet
 */

#include <sequence/DisplayUtils.h>

#include <sequence/BrowseItem.h>
#include "PlaylistBuilder.h"

using namespace std;
using namespace sequence;

namespace duke {
namespace protocol {

static inline void set(FrameRange* pRange, const Range &range) {
    pRange->set_first(range.first);
    pRange->set_last(range.last);
}

TrackBuilder::TrackBuilder(Track &track, const char *name, unsigned int recStart) : track(track), currentRec(recStart) {
    track.set_name(name);
}

Range TrackBuilder::getRecordRangeAndUpdate(unsigned int count, const unsigned int offset) {
    const unsigned int record = offset == UINT_MAX ? currentRec : offset;
    Range recordRange = Range::weak(record, count);
    const unsigned int newRecord = record + count;
    if(newRecord<=currentRec)
        throw logic_error("Record insertion point is getting back in time possibly overlapping previous Shots. Please declare your Shots in record order");
    currentRec = record + count;
    return recordRange;
}

Clip& TrackBuilder::addBrowseItem(const sequence::BrowseItem &item, const unsigned int offset) {
    switch (item.type) {
        case sequence::SEQUENCE:{
            string filename = (item.path/item.sequence.pattern.string()).make_preferred().string();
            return addSequence(filename.c_str(), getRecordRangeAndUpdate(item.sequence.range.duration(), offset), item.sequence.range);
        }
        case sequence::UNITFILE:{
            string filename = boost::filesystem::path(item.path).make_preferred().string();
            return addImage(filename.c_str(), getRecordRangeAndUpdate(1, offset));
        }
        case sequence::FOLDER:
        case sequence::UNDEFINED:
        default:
            break;
    }
    throw logic_error("TrackBuilder : Invalid browse item type");
}

Clip& TrackBuilder::addImage(const char *filename, const Range &record) {
    Clip *pClip = track.add_clip();
    set(pClip->mutable_record(), record);
    Media &media = *pClip->mutable_media();
    media.set_type(Media_Type_SINGLE_IMAGE);
    media.set_filename(filename);
    return *pClip;
}

Clip& TrackBuilder::addMedia(const char *filename, const Range&record, const Range &source, const Media_Type mediaType) {
    Clip *pClip = track.add_clip();
    set(pClip->mutable_record(), record);
    Media &media = *pClip->mutable_media();
    media.set_type(mediaType);
    media.set_filename(filename);
    set(media.mutable_source(), source);
    return *pClip;
}

Clip& TrackBuilder::addMedia(const char *filename, const Range &record, const unsigned int offset, const Media_Type mediaType) {
    return addMedia(filename, record, Range(record.first + offset, record.last + offset), mediaType);
}

TrackBuilder PlaylistBuilder::addTrack(const char *trackName, unsigned int recStart) {
    return TrackBuilder(*playlist.add_track(), trackName, recStart);
}

PlaylistBuilder::operator ::duke::protocol::Playlist() {
    return playlist;
}

} // namespace protocol
} /* namespace duke */
