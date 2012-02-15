#ifndef PLAYLISTBUILDER_H_
#define PLAYLISTBUILDER_H_

#include <player.pb.h>

namespace sequence {
struct Range;
}  // namespace sequence

namespace duke {
namespace protocol {

using sequence::Range;

struct TrackBuilder {
    Track &track;
    TrackBuilder(Track &track, const char *name);
    Media& addImage(const char *filename, const Range &record);
    inline Media& addMovie(const char *filename, const Range &record, const Range &source) {
        return addMedia(filename, record, source, Media_Type_MOVIE_CONTAINER);
    }
    inline Media& addMovie(const char *filename, const Range &record, const uint32_t offset) {
        return addMedia(filename, record, offset, Media_Type_MOVIE_CONTAINER);
    }
    inline Media& addSequence(const char *filename, const Range &record, const Range &source) {
        return addMedia(filename, record, source, Media_Type_IMAGE_SEQUENCE);
    }
    inline Media& addSequence(const char *filename, const Range &record, const uint32_t offset) {
        return addMedia(filename, record, offset, Media_Type_IMAGE_SEQUENCE);
    }
private:
    Media& addMedia(const char *filename, const Range &record, const Range &source, const Media_Type mediaType);
    Media& addMedia(const char *filename, const Range &record, const uint32_t offset, const Media_Type mediaType);
};

struct PlaylistBuilder {
    TrackBuilder addTrack(const char *trackName);
    operator Playlist();
private:
    Playlist playlist;
};

} // namespace protocol
} /* namespace duke */
#endif /* PLAYLISTBUILDER_H_ */
