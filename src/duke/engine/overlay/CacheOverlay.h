/*
 * CacheOverlay.h
 *
 *  Created on: Feb 21, 2013
 *      Author: Nicolas Rondaud
 */

#ifndef CACHEOVERLAY_H_
#define CACHEOVERLAY_H_

#include "IOverlay.h"
#include <duke/engine/Timeline.h>

namespace duke {

struct Context;
struct GlyphRenderer;

class CacheOverlay: public duke::IOverlay {
public:
	CacheOverlay(const std::shared_ptr<GlyphRenderer>&,
			const std::map<const IMediaStream*, std::vector<Range> > &,
			const Timeline& timeline);
	virtual void render(const Context&) const;

private:
	const std::shared_ptr<GlyphRenderer> m_pGlyphRenderer;
	const std::map<const IMediaStream*, std::vector<Range> > & m_CacheState;
	const Timeline& m_timeline;
};

} /* namespace duke */
#endif /* CACHEOVERLAY_H_ */
