#pragma once

#include <duke/engine/cache/PboPackedFrame.hpp>
#include <duke/engine/cache/TimelineIterator.hpp>
#include <duke/gl/GlObjects.hpp>
#include <duke/engine/cache/PboPool.hpp>
#include <duke/NonCopyable.hpp>

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
