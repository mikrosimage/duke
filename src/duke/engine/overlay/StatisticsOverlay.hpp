#pragma once

#include "IOverlay.hpp"
#include <duke/engine/Timeline.hpp>
#include <duke/time/Clock.hpp>

namespace duke {

struct Context;
struct GlyphRenderer;

class StatisticsOverlay : public duke::IOverlay {
 public:
  StatisticsOverlay(const GlyphRenderer&, const Timeline& timeline);

  virtual void render(const Context&) const;

  std::map<const IMediaStream*, std::vector<Range> > cacheState;
  Metronom vBlankMetronom;
  Metronom frameMetronom;

 private:
  const GlyphRenderer& m_GlyphRenderer;
  const Timeline& m_Timeline;
};

} /* namespace duke */
