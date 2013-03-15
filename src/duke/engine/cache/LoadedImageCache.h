/*
 * ImageCache.h
 *
 *  Created on: Feb 3, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef IMAGECACHE_H_
#define IMAGECACHE_H_

#include <duke/NonCopyable.h>
#include <duke/engine/cache/TimelineIterator.h>
#include <duke/engine/Timeline.h>
#include <duke/imageio/RawPackedFrame.h>
#include <concurrent/cache/lookahead_cache.hpp>
#include <thread>
#include <vector>

namespace duke {

struct LoadedImageCache: public noncopyable {
	LoadedImageCache(unsigned workerThreadDefault, size_t maxSizeDefault);
	~LoadedImageCache();

	void setWorkerCount(size_t workerCount);
	void load(const Timeline& timeline);
	void cue(size_t frame, IterationMode mode);
	void terminate();

	bool get(const MediaFrameReference &id, RawPackedFrame &data) const;
	uint64_t dumpState(std::map<const IMediaStream*, std::vector<Range> > &state) const;
	uint64_t getMaxWeight() const;
	size_t getWorkerCount() const;

private:
	void startWorkers();
	void stopWorkers();
	void workerFunction();
	std::string& workerStep(MediaFrameReference&, std::string&, std::string&);

	typedef MediaFrameReference ID_TYPE;
	typedef uint64_t METRIC_TYPE;
	typedef RawPackedFrame DATA_TYPE;
	typedef TimelineIterator WORK_UNIT_RANGE;

	size_t m_MaxWeight;
	concurrent::cache::lookahead_cache<ID_TYPE, METRIC_TYPE, DATA_TYPE, WORK_UNIT_RANGE> m_Cache;
	std::vector<std::thread> m_WorkerThreads;
	Timeline m_Timeline;
	Ranges m_MediaRanges;
	size_t m_WorkerCount;

	mutable std::vector<MediaFrameReference> m_DumpStateTmp;
};

} /* namespace duke */
#endif /* IMAGECACHE_H_ */
