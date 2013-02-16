/*
 * TextureCache.cpp
 *
 *  Created on: Feb 11, 2013
 *      Author: Guillaume Chatelet
 */

#include "LoadedTextureCache.h"
#include <duke/cmdline/CmdLineParameters.h>
#include <duke/MapRemoveIf.h>

namespace duke {

LoadedTextureCache::LoadedTextureCache(const CmdLineParameters &parameters) :
		m_ImageCache(parameters.workerThreadDefault, parameters.imageCacheSizeDefault), m_LastFrame(0) {
}

void LoadedTextureCache::load(const Timeline& timeline) {
	m_Timeline = timeline;
	m_TimelineRanges = getMediaRanges(m_Timeline);
	m_ImageCache.load(timeline);
}

void LoadedTextureCache::ensureReady(size_t frame) {
	if (frame != m_LastFrame) {
		m_ImageCache.cue(frame);
		m_LastFrame = frame;
	}
	m_FrameMedia.clear();
	for (TimelineIterator itr(&m_Timeline, &m_TimelineRanges, frame); !itr.empty() && frame == itr.getCurrentFrame();) {
		const auto mfr = itr.next();
		m_FrameMedia.insert(mfr);
		if (m_Map.find(mfr) != m_Map.end())
			continue;
		PboPackedFrame pboPackedFrame;
		const auto pboReady = m_PboCache.get(m_ImageCache, mfr, pboPackedFrame);
		if (pboReady)
			m_Map.insert( { mfr, TexturePackedFrame(pboPackedFrame, m_TexturePool.get(pboPackedFrame.description)) });
	}
	// removing every key not used in this frame
	const auto isOutsideCurrentFrame = [&](const Map::value_type &pair) {
		return m_FrameMedia.find(pair.first)==end(m_FrameMedia);
	};
	map_erase_if(m_Map, isOutsideCurrentFrame);
}

const Timeline& LoadedTextureCache::getTimeline() const {
	return m_Timeline;
}

const TexturePackedFrame* LoadedTextureCache::getLoadedTexture(const MediaFrameReference &mfr) const {
	auto pFound = m_Map.find(mfr);
	if (pFound == m_Map.end())
		return nullptr;
	return &pFound->second;
}
} /* namespace duke */
