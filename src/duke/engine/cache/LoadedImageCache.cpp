#include "LoadedImageCache.hpp"
#include <duke/attributes/AttributeKeys.hpp>
#include <duke/engine/streams/IMediaStream.hpp>
#include <duke/engine/ImageLoadUtils.hpp>
#include <duke/memory/Allocator.hpp>
#include <duke/filesystem/FsUtils.hpp>

namespace duke {

LoadedImageCache::LoadedImageCache(unsigned workerThreadDefault, size_t maxSizeDefault) :
		m_MaxWeight(maxSizeDefault), m_Cache(m_MaxWeight), m_WorkerCount(workerThreadDefault) {
}

LoadedImageCache::~LoadedImageCache() {
	stopWorkers();
}

void LoadedImageCache::setWorkerCount(size_t workerCount) {
	if (workerCount == m_WorkerCount)
		return;
	stopWorkers();
	m_WorkerCount = workerCount;
	startWorkers();
}

void LoadedImageCache::load(const Timeline& timeline) {
	stopWorkers();
	m_Timeline = timeline;
	m_MediaRanges = getMediaRanges(m_Timeline);
	if (m_MediaRanges.empty())
		return;
	startWorkers();
	cue(m_MediaRanges.begin()->first, IterationMode::PINGPONG);
}

void LoadedImageCache::cue(size_t frame, IterationMode mode) {
	m_Cache.process(TimelineIterator(&m_Timeline, &m_MediaRanges, frame, mode));
}

void LoadedImageCache::terminate() {
	stopWorkers();
}

bool LoadedImageCache::get(const MediaFrameReference &id, RawPackedFrame &data) const {
	return m_Cache.get(id, data);
}

uint64_t LoadedImageCache::dumpState(std::map<const IMediaStream*, std::vector<Range> > &state) const {
	state.clear();

	const auto currentWeight = m_Cache.dumpKeys(m_DumpStateTmp);
	std::sort(begin(m_DumpStateTmp), end(m_DumpStateTmp));

	const IMediaStream *pLastMedia = nullptr;
	std::vector<Range> mediaRanges;

	for (const auto &key : m_DumpStateTmp) {
		const IMediaStream *pCurrentMedia = key.first;
		const size_t frame = key.second;
		const bool newMedia = pCurrentMedia != pLastMedia;
		if (newMedia) {
			if (pLastMedia != nullptr)
				state.insert(std::make_pair(pLastMedia, std::move(mediaRanges)));
			mediaRanges.clear();
			mediaRanges.emplace_back(frame, frame);
		} else {
			if (mediaRanges.back().last + 1 == frame)
				++mediaRanges.back().last;
			else
				mediaRanges.emplace_back(frame, frame);
		}
		pLastMedia = pCurrentMedia;
	}
	if (pLastMedia != nullptr)
		state.insert(std::make_pair(pLastMedia, std::move(mediaRanges)));

	return currentWeight;
}

uint64_t LoadedImageCache::getMaxWeight() const {
	return m_MaxWeight;
}

size_t LoadedImageCache::getWorkerCount() const {
	return m_WorkerCount;
}

void LoadedImageCache::startWorkers() {
	if (!m_WorkerThreads.empty())
		throw std::logic_error("You must stop workers thread before calling startWorkers");
	m_Cache.terminate(false);
	for (size_t i = 0; i < m_WorkerCount; ++i)
		m_WorkerThreads.emplace_back(&LoadedImageCache::workerFunction, this);
}

void LoadedImageCache::stopWorkers() {
	m_Cache.terminate(true);
	for (std::thread &thread : m_WorkerThreads)
		thread.join();
	m_WorkerThreads.clear();
}

void LoadedImageCache::workerFunction() {
	MediaFrameReference mfr;
	std::string path;
	std::string error;
	try {
		for (;;) {
			workerStep(mfr, path, error);
			if (!error.empty()) {
				printf("error while reading %s : %s\n", path.c_str(), error.c_str());
				m_Cache.push(mfr, 1UL, RawPackedFrame());
			} else {
//				printf("successfully loaded %s\n", path.c_str());
			}
		}
	} catch (concurrent::terminated&) {
	} catch (std::exception &e) {
		printf("Something bad happened while reading image : %s\n", e.what());
	}
}

namespace {

AlignedMalloc gAlignedMallocator;
BigAlignedBlock gBigAlignedMallocator;

}  // namespace

std::string& LoadedImageCache::workerStep(MediaFrameReference &mfr, std::string& path, std::string& error) {
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
	duke::load(path.c_str(), pExtension, [&](RawPackedFrame&& packedFrame, const void* pVolatileData) {
		const size_t dataSize = packedFrame.description.dataSize;
		if(!packedFrame.pData) {
			packedFrame.pData = make_shared_memory<char>(dataSize, gBigAlignedMallocator);
			memcpy(packedFrame.pData.get(), pVolatileData, dataSize);
		}
		packedFrame.attributes.emplace_back(attribute::pDukeFileExtensionKey,pExtension);
		auto lastSlashIndex = path.rfind('/');
		const char* pFilename = lastSlashIndex!=std::string::npos ? path.c_str()+lastSlashIndex+1 : path.c_str();
		packedFrame.attributes.emplace_back(attribute::pDukeFileNameKey,pFilename);
		m_Cache.push(mfr, dataSize, std::move(packedFrame));
	}, error);
	return error;
}

} /* namespace duke */
