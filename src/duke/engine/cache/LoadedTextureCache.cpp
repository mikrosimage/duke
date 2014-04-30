#include "LoadedTextureCache.hpp"
#include <duke/cmdline/CmdLineParameters.hpp>
#include <algorithm>

namespace duke {

template <typename Map, typename F>
void map_erase_if(Map& m, F pred) {
  for (auto i = std::begin(m); (i = std::find_if(i, std::end(m), pred)) != std::end(m);) m.erase(i++);
}

LoadedTextureCache::LoadedTextureCache(const CmdLineParameters& parameters)
    : m_ImageCache(parameters.workerThreadDefault, parameters.imageCacheSizeDefault), m_LastFrame(0) {}

void LoadedTextureCache::load(const Timeline& timeline) {
  m_Timeline = timeline;
  m_TimelineRanges = getMediaRanges(m_Timeline);
  m_ImageCache.load(timeline);
}

void LoadedTextureCache::prepare(size_t frame, IterationMode mode) {
  if (frame != m_LastFrame) {
    m_ImageCache.cue(frame, mode);
    m_LastFrame = frame;
  }
  m_FrameMedia.clear();
  TimelineIterator itr(&m_Timeline, &m_TimelineRanges, frame, IterationMode::FORWARD);
  itr.setMaxFrameIterations(2);  // loading this frame and prefetching next one
  for (; !itr.empty();) {
    const auto mfr = itr.next();
    m_FrameMedia.insert(mfr);
    if (m_Map.find(mfr) != m_Map.end())  // already in cache
      continue;
    PboPackedFrame pboPackedFrame;
    const auto pboReady = m_PboCache.get(m_ImageCache, mfr, pboPackedFrame);
    if (pboReady)
      m_Map.insert({mfr, TexturePackedFrame(pboPackedFrame, m_TexturePool.get(pboPackedFrame.description))});
  }
  // discarding all textures expect those fetched during this call
  const auto isOutsideCurrentFrame = [&](const Map::value_type& pair) {
    return m_FrameMedia.find(pair.first) == end(m_FrameMedia);
  };
  map_erase_if(m_Map, isOutsideCurrentFrame);
}

const Timeline& LoadedTextureCache::getTimeline() const { return m_Timeline; }

const LoadedImageCache& LoadedTextureCache::getImageCache() const { return m_ImageCache; }

const TexturePackedFrame* LoadedTextureCache::getLoadedTexture(const MediaFrameReference& mfr) const {
  auto pFound = m_Map.find(mfr);
  if (pFound == m_Map.end()) return nullptr;
  return &pFound->second;
}
} /* namespace duke */
