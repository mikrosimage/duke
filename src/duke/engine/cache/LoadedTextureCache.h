/*
 * LoadedTextureCache.h
 *
 *  Created on: Feb 11, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef LOADEDTEXTURECACHE_H_
#define LOADEDTEXTURECACHE_H_

#include <duke/NonCopyable.h>
#include <duke/engine/cache/LoadedImageCache.h>
#include <duke/engine/cache/LoadedPboCache.h>
#include <duke/engine/cache/TexturePackedFrame.h>
#include <duke/engine/cache/TexturePool.h>
#include <duke/engine/Timeline.h>
#include <map>
#include <vector>

namespace duke {

struct CmdLineParameters;

struct LoadedTextureCache: public noncopyable {
	LoadedTextureCache(const CmdLineParameters &parameters);

	void load(const Timeline& timeline);
	void prepare(size_t frame, IterationMode mode);

	const TexturePackedFrame* getLoadedTexture(const MediaFrameReference &mfr) const;
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
#endif /* LOADEDTEXTURECACHE_H_ */
