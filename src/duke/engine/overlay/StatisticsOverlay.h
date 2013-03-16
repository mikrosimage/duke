/*
 * CacheOverlay.h
 *
 *  Created on: Feb 21, 2013
 *      Author: Nicolas Rondaud
 */

#pragma once

#include "IOverlay.h"
#include <duke/engine/Timeline.h>

namespace duke {

struct Context;
struct GlyphRenderer;

class StatisticsOverlay: public duke::IOverlay {
public:
	StatisticsOverlay(const GlyphRenderer&, const std::map<const IMediaStream*, std::vector<Range> > &, const Timeline& timeline);
	virtual void render(const Context&) const;

private:
	const GlyphRenderer &m_GlyphRenderer;
	const std::map<const IMediaStream*, std::vector<Range> > & m_CacheState;
	const Timeline& m_Timeline;
};

} /* namespace duke */
