/*
 * PlaylistHelper.h
 *
 *  Created on: 8 fevr. 2012
 *      Author: Guillaume Chatelet
 */

#ifndef PLAYLISTHELPER_H_
#define PLAYLISTHELPER_H_

#include <player.pb.h>
#include <sequence/BrowseItem.h>

#include <vector>

namespace duke {
namespace protocol {

struct PlaylistError : public std::runtime_error {
    PlaylistError(const std::string &msg) : std::runtime_error(msg) {}
};

using sequence::Range;
typedef std::vector<Range> Ranges;
typedef Ranges::const_iterator RangeCItr;

struct TrackHelper {
    Ranges recRanges;
    Range range;
    Track track;
    std::vector<sequence::BrowseItem> items;
    TrackHelper(const Track &track);
    unsigned int index(const unsigned int frame) const;
    const RangeCItr rangeContaining(const unsigned int frame) const;
    const unsigned int clipIndexContaining(const unsigned int frame) const;
    const bool contains(const unsigned int frame) const;
};

struct PlaylistIndex {
    unsigned int frame;
    uint16_t track;
    explicit PlaylistIndex(unsigned int frame, uint16_t track) :  frame(frame), track(track) {}
    inline bool operator<(const PlaylistIndex& other) const {
        return frame == other.frame ? track < other.track : frame < other.frame;
    }
};

struct MediaFrame {
    PlaylistIndex index;
    unsigned int source;
    Media_Type type;
    sequence::BrowseItem item;
    MediaFrame() : index(0,0),source(0),type(Media_Type_SINGLE_IMAGE){}
    MediaFrame(const PlaylistIndex &index, const unsigned int source, const Media_Type type, const sequence::BrowseItem &item) : index(index),source(source),type(type),item(item){}
    std::string filename() const;
};

typedef std::vector<MediaFrame> MediaFrames;
typedef std::vector<Clip> Clips;

struct PlaylistHelper {
    Playlist playlist;
    Range range;
    std::vector<TrackHelper> tracks;
    Ranges allClips;
    PlaylistHelper();
    PlaylistHelper(const Playlist &playlist);
    bool empty() const;
    unsigned int index(const unsigned int frame) const;
    uint16_t tracksAt(const unsigned int frame) const;
    void mediaFramesAt(const unsigned int frame, MediaFrames &frames) const;
    void clipsAt(const unsigned int frame, Clips &clips) const;
};

} /* namespace protocol */
} /* namespace duke */
#endif /* PLAYLISTHELPER_H_ */
