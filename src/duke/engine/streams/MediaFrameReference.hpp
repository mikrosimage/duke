#pragma once

#include <utility>
#include <cstddef>

namespace duke {

class IMediaStream;

struct MediaFrameReference {
  const IMediaStream* pStream;
  size_t frame;

  MediaFrameReference(const IMediaStream* pStream, size_t frame) : pStream(pStream), frame(frame) {}
  MediaFrameReference() : pStream(nullptr), frame(0) {}
  bool operator==(const MediaFrameReference& other) const { return asPair() == other.asPair(); }
  bool operator!=(const MediaFrameReference& other) const { return asPair() != other.asPair(); }
  bool operator<(const MediaFrameReference& other) const { return asPair() < other.asPair(); }

 private:
  inline std::pair<const IMediaStream*, size_t> asPair() const { return std::make_pair(pStream, frame); }
};

}  // namespace duke
