/*
 * PboCache.h
 *
 *  Created on: Feb 11, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef PBOCACHE_H_
#define PBOCACHE_H_

#include <duke/engine/cache/PboPackedFrame.h>
#include <duke/engine/cache/TimelineIterator.h>
#include <duke/gl/GlObjects.h>
#include <duke/engine/cache/PboPool.h>
#include <duke/NonCopyable.h>

namespace duke {

struct LoadedImageCache;

struct LoadedPboCache: public noncopyable {
	bool get(const LoadedImageCache& imageCache, const MediaFrameReference& mfr, PboPackedFrame &pbo);

private:
	void moveFront(const MediaFrameReference& mfr);
	void evictOneIfNecessary();

	const size_t m_MaxCount = 10;
	PboPool m_PboPool;
	std::map<MediaFrameReference, PboPackedFrame> m_Map;
	std::vector<MediaFrameReference> m_Fifo;
};

} /* namespace duke */
#endif /* PBOCACHE_H_ */
