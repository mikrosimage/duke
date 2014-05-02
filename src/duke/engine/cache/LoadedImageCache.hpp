#pragma once

#include <duke/base/NonCopyable.hpp>
#include <duke/engine/cache/TimelineIterator.hpp>
#include <duke/engine/Timeline.hpp>
#include <duke/engine/streams/IMediaStream.hpp>
#include <duke/imageio/FrameData.hpp>
#include <concurrent/cache/lookahead_cache.hpp>
#include <thread>
#include <vector>

namespace duke {

struct LoadedImageCache : public noncopyable {
  LoadedImageCache(unsigned workerThreadDefault, size_t maxSizeDefault);
  ~LoadedImageCache();

  void setWorkerCount(size_t workerCount);
  void load(const Timeline &timeline);
  void cue(size_t frame, IterationMode mode);
  void terminate();

  bool get(const MediaFrameReference &id, FrameData &data) const;
  uint64_t dumpState(std::map<const IMediaStream *, std::vector<Range> > &state) const;
  uint64_t getMaxWeight() const;
  size_t getWorkerCount() const;

 private:
  void startWorkers();
  void stopWorkers();
  void workerFunction();

  typedef MediaFrameReference ID_TYPE;
  typedef uint64_t METRIC_TYPE;
  typedef FrameData DATA_TYPE;
  typedef TimelineIterator WORK_UNIT_RANGE;

  size_t m_MaxWeight;
  concurrent::cache::lookahead_cache<ID_TYPE, METRIC_TYPE, DATA_TYPE, WORK_UNIT_RANGE> m_Cache;
  std::vector<std::thread> m_WorkerThreads;
  Timeline m_Timeline;
  Ranges m_MediaRanges;
  bool m_TimelineHasMovie;
  size_t m_WorkerCount;

  mutable std::vector<MediaFrameReference> m_DumpStateTmp;
};

} /* namespace duke */
