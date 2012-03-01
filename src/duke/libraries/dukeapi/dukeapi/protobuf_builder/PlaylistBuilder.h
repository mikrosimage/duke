#ifndef PLAYLISTBUILDER_H_
#define PLAYLISTBUILDER_H_

#include <player.pb.h>

namespace sequence {
struct Range;
struct BrowseItem;
}  // namespace sequence

namespace duke {
namespace protocol {

using sequence::Range;

struct TrackBuilder {
    Track &track;
    TrackBuilder(Track &track, const char *name, unsigned int recStart = 0);

    Clip& addBrowseItem(const sequence::BrowseItem &item, const unsigned int offset=UINT_MAX);

    Clip& addImage(const char *filename, const Range &record);

    inline Clip& addMovie(const char *filename, const Range &record, const Range &source) {
        return addMedia(filename, record, source, Media_Type_MOVIE_CONTAINER);
    }

    inline Clip& addMovie(const char *filename, const Range &record, const unsigned int offset) {
        return addMedia(filename, record, offset, Media_Type_MOVIE_CONTAINER);
    }

    inline Clip& addSequence(const char *filename, const Range &record, const Range &source) {
        return addMedia(filename, record, source, Media_Type_IMAGE_SEQUENCE);
    }

    inline Clip& addSequence(const char *filename, const Range &record, const unsigned int offset) {
        return addMedia(filename, record, offset, Media_Type_IMAGE_SEQUENCE);
    }
    unsigned int currentRecord()const{return currentRec;}
private:
    unsigned int currentRec;
    Clip& addMedia(const char *filename, const Range &record, const Range &source, const Media_Type mediaType);
    Clip& addMedia(const char *filename, const Range &record, const unsigned int offset, const Media_Type mediaType);
    Range getRecordRangeAndUpdate(unsigned int count, const unsigned int offset);
};

struct PlaylistBuilder {
    TrackBuilder addTrack(const char *trackName, unsigned int recStart = 0);
    operator Playlist();
private:
    Playlist playlist;
};

} // namespace protocol
} /* namespace duke */
#endif /* PLAYLISTBUILDER_H_ */
