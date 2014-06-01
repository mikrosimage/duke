#pragma once

#include "duke/base/NonCopyable.hpp"
#include "duke/engine/cache/LoadedImageCache.hpp"
#include "duke/engine/cache/LoadedPboCache.hpp"
#include "duke/engine/cache/TexturePackedFrame.hpp"
#include "duke/engine/cache/TexturePool.hpp"
#include "duke/engine/Timeline.hpp"
#include <map>
#include <vector>

namespace duke {

struct CmdLineParameters;

struct LoadedTextureCache : public noncopyable {
  LoadedTextureCache(const CmdLineParameters& parameters);

  void load(const Timeline& timeline);
  void prepare(size_t frame, IterationMode mode);

  const TexturePackedFrame* getLoadedTexture(const MediaFrameReference& mfr) const;
  const Timeline& getTimeline() const;
  const LoadedImageCache& getImageCache() const;

 private:
  Timeline m_Timeline;
  Ranges m_TimelineRanges;
  LoadedImageCache m_ImageCache;
  LoadedPboCache m_PboCache;
  TexturePool m_TexturePool;
  size_t m_LastFrame;
  std::set<MediaFrameReference> m_FrameMedia;
  typedef std::map<MediaFrameReference, TexturePackedFrame> Map;
  Map m_Map;
};

} /* namespace duke */
