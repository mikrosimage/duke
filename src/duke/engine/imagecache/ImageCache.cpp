/*
 * ImageCache.cpp
 *
 *  Created on: Feb 3, 2013
 *      Author: Guillaume Chatelet
 */

#include "ImageCache.h"
#include <duke/engine/streams/IMediaStream.h>
#include <duke/engine/ImageLoadUtils.h>
#include <duke/memory/Allocator.h>
#include <duke/filesystem/FsUtils.h>

namespace duke {

ImageCache::ImageCache() :
		m_Cache(500 * 1024 * 1024), m_WorkerCount(1) {
}

ImageCache::~ImageCache() {
	stopWorkers();
}

void ImageCache::setWorkerCount(size_t workerCount) {
	if (workerCount == m_WorkerCount)
		return;
	stopWorkers();
	m_WorkerCount = workerCount;
	startWorkers();
}

void ImageCache::load(const Timeline& timeline) {
	stopWorkers();
	m_Timeline = timeline;
	m_MediaRanges = getMediaRanges(m_Timeline);
	if (m_MediaRanges.empty())
		return;
	startWorkers();
	m_Cache.process(TimelineIterator(&m_Timeline, &m_MediaRanges, m_MediaRanges.begin()->first));
}

void ImageCache::terminate() {
	stopWorkers();
}

bool ImageCache::get(const MediaFrameReference &id, PackedFrame &data) const {
	return m_Cache.get(id, data);
}

size_t ImageCache::getWorkerCount() const {
	return m_WorkerCount;
}

void ImageCache::startWorkers() {
	if (!m_WorkerThreads.empty())
		throw std::logic_error("You must stop workers thread before calling startWorkers");
	m_Cache.terminate(false);
	for (size_t i = 0; i < m_WorkerCount; ++i)
		m_WorkerThreads.emplace_back(&ImageCache::workerFunction, this);
}

void ImageCache::stopWorkers() {
	m_Cache.terminate(true);
	for (std::thread &thread : m_WorkerThreads)
		thread.join();
	m_WorkerThreads.clear();
}

void ImageCache::workerFunction() {
	MediaFrameReference mfr;
	std::string path;
	std::string error;
	try {
		for (;;) {
			workerStep(mfr, path, error);
			if (!error.empty()) {
				printf("error while reading %s : %s\n", path.c_str(), error.c_str());
				m_Cache.push(mfr, 1UL, PackedFrame());
			} else {
				printf("successfully loaded %s\n", path.c_str());
			}
		}
	} catch (concurrent::terminated&) {
	} catch (std::exception &e) {
		printf("Something bad happened while reading image : %s\n", e.what());
	}
}

static AlignedMalloc gAllocator; //FIXME this global allocator will suffer from congestion, use per thread allocators

std::string& ImageCache::workerStep(MediaFrameReference &mfr, std::string& path, std::string& error) {
	error.clear();
	m_Cache.pop(mfr);
	const IMediaStream *pStream = mfr.first;
	if (!pStream)
		return error = "empty stream";
	pStream->generateFilePath(path, mfr.second);
	if (path.empty())
		return error = "stream has no path";
	const char* pExtension = fileExtension(path.c_str());
	if (!pExtension)
		return error = "stream has no extension";
	duke::load(path.c_str(), pExtension, [&](PackedFrame&& packedFrame, const void* pVolatileData) {
		const size_t dataSize = packedFrame.description.dataSize;
		if(!packedFrame.pData) {
			packedFrame.pData = make_shared_memory<char>(dataSize, gAllocator);
			memcpy(packedFrame.pData.get(), pVolatileData, dataSize);
		}
		m_Cache.push(mfr, dataSize, std::move(packedFrame));
	}, error);
	return error;
}

} /* namespace duke */
