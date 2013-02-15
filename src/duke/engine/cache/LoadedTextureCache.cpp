/*
 * TextureCache.cpp
 *
 *  Created on: Feb 11, 2013
 *      Author: Guillaume Chatelet
 */

#include "LoadedTextureCache.h"

namespace duke {

void LoadedTextureCache::load(const Timeline& timeline) {
	m_Timeline = timeline;
	m_TimelineRanges = getMediaRanges(m_Timeline);
	m_ImageCache.load(timeline);
}

void LoadedTextureCache::prepare(size_t frame) {
	fillMediaFor(frame);
	m_PboCache.fill(m_ImageCache, frameMedia);
}

const Timeline& LoadedTextureCache::getTimeline() const {
	return m_Timeline;
}

void LoadedTextureCache::fillMediaFor(size_t frame) {
	frameMedia.clear();
	for (TimelineIterator itr(&m_Timeline, &m_TimelineRanges, frame); !itr.empty() && frame == itr.getCurrentFrame();)
		frameMedia.push_back(itr.next());
}
} /* namespace duke */
