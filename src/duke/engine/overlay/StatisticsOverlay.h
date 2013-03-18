/*
 * CacheOverlay.h
 *
 *  Created on: Feb 21, 2013
 *      Author: Nicolas Rondaud
 */

#pragma once

#include "IOverlay.h"
#include <duke/engine/Timeline.h>
#include <duke/time/Clock.h>

namespace duke {

struct Context;
struct GlyphRenderer;

class StatisticsOverlay: public duke::IOverlay {
public:
	StatisticsOverlay(const GlyphRenderer&, const Timeline& timeline);


	virtual void render(const Context&) const;

	std::map<const IMediaStream*, std::vector<Range> > cacheState;
	Metronom vBlankMetronom;
	Metronom frameMetronom;
private:
	const GlyphRenderer &m_GlyphRenderer;
	const Timeline& m_Timeline;
};

} /* namespace duke */
