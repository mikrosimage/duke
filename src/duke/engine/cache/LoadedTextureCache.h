/*
 * LoadedTextureCache.h
 *
 *  Created on: Feb 11, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef LOADEDTEXTURECACHE_H_
#define LOADEDTEXTURECACHE_H_

#include <duke/NonCopyable.h>
#include <duke/engine/cache/ImageCache.h>
#include <duke/engine/cache/PboCache.h>
#include <duke/engine/Timeline.h>

namespace duke {

struct LoadedTextureCache: public noncopyable {
	void load(const Timeline& timeline);
	void prepare(size_t frame);
	const Timeline& getTimeline() const;
private:
	void fillMediaFor(size_t frame);

	Timeline m_Timeline;
	Ranges m_TimelineRanges;
	ImageCache m_ImageCache;
	PboCache m_PboCache;
	std::vector<MediaFrameReference> frameMedia;
};

} /* namespace duke */
#endif /* LOADEDTEXTURECACHE_H_ */
