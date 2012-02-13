#ifndef PLAYLISTBUILDER_H_
#define PLAYLISTBUILDER_H_

#include <player.pb.h>
#include <sequence/Range.h>

namespace duke {
namespace protocol {

namespace range {
FrameRange make(uint32_t first, uint32_t last);
} // namespace range

struct TrackBuilder {
    Track &track;
    TrackBuilder(Track &track, const char *name);
    Media& addImage(const char *filename, const FrameRange record);
    Media& addMovie(const char *filename, const FrameRange record, const FrameRange source) {
        return addMedia(filename, record, source, Media_Type_MOVIE_CONTAINER);
    }
    Media& addMovie(const char *filename, const FrameRange record, const uint32_t offset) {
        return addMedia(filename, record, offset, Media_Type_MOVIE_CONTAINER);
    }
    Media& addSequence(const char *filename, const FrameRange record, const FrameRange source) {
        return addMedia(filename, record, source, Media_Type_IMAGE_SEQUENCE);
    }
    Media& addSequence(const char *filename, const FrameRange record, const uint32_t offset) {
        return addMedia(filename, record, offset, Media_Type_IMAGE_SEQUENCE);
    }
private:
    Media& addMedia(const char *filename, const FrameRange record, const FrameRange source, const Media_Type mediaType);
    Media& addMedia(const char *filename, const FrameRange record, const uint32_t offset, const Media_Type mediaType);
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
