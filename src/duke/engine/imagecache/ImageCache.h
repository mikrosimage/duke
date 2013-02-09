/*
 * ImageCache.h
 *
 *  Created on: Feb 3, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef IMAGECACHE_H_
#define IMAGECACHE_H_

#include <duke/NonCopyable.h>
#include <duke/engine/imagecache/TimelineIterator.h>
#include <duke/engine/Timeline.h>
#include <duke/imageio/PackedFrame.h>
#include <concurrent/cache/lookahead_cache.hpp>
#include <thread>
#include <vector>

namespace duke {

class ImageCache: public noncopyable {
public:
	ImageCache();
	~ImageCache();

	void setWorkerCount(size_t workerCount);
	void load(const Timeline& timeline);
	void terminate();

	bool get(const MediaFrameReference &id, PackedFrame &data) const;
	size_t getWorkerCount() const;

private:
	void startWorkers();
	void stopWorkers();
	void workerFunction();
	std::string& workerStep(MediaFrameReference&, std::string&, std::string&);
	typedef MediaFrameReference ID_TYPE;
	typedef uint64_t METRIC_TYPE;
	typedef PackedFrame DATA_TYPE;
	typedef TimelineIterator WORK_UNIT_RANGE;
	concurrent::cache::lookahead_cache<ID_TYPE, METRIC_TYPE, DATA_TYPE, WORK_UNIT_RANGE> m_Cache;
	std::vector<std::thread> m_WorkerThreads;
	Timeline m_Timeline;
	Ranges m_MediaRanges;
	size_t m_WorkerCount;
};

} /* namespace duke */
#endif /* IMAGECACHE_H_ */
