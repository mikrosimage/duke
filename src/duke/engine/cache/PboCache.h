/*
 * PboCache.h
 *
 *  Created on: Feb 11, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef PBOCACHE_H_
#define PBOCACHE_H_

#include <duke/engine/cache/TimelineIterator.h>
#include <duke/gl/GlObjects.h>
#include <duke/engine/cache/PboPool.h>
#include <duke/NonCopyable.h>
#include <memory>
#include <vector>

namespace duke {

struct ImageCache;

struct PboCache: public noncopyable {
	void fill(const ImageCache& cache, const std::vector<MediaFrameReference>& mfrs);
	bool get(const MediaFrameReference& mfr, SharedPbo &pbo) const;

private:
	bool has(const MediaFrameReference& mfr) const;

	PboPool m_PboPool;
	std::map<MediaFrameReference, SharedPbo> m_Map;
};

} /* namespace duke */
#endif /* PBOCACHE_H_ */
