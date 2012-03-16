/*
 * SmartCache.cpp
 *
 *  Created on: 9 nov. 2011
 *      Author: Guillaume Chatelet
 */

#include "SmartCache.h"

#include "ImageHolder.h"
#include "ImageToolbox.h"

#include <dukeengine/host/io/ImageDecoderFactory.h>
#include <dukeengine/utils/CacheIterator.h>

#include <concurrent/ConcurrentQueue.hpp>
#include <concurrent/cache/LookAheadCache.hpp>
#include <dukeapi/sequence/PlaylistHelper.h>

#include <boost/thread.hpp>

#include <set>
#include <iterator>
#include <iostream>

#include <cassert>

using namespace std;
using namespace concurrent::cache;
using namespace duke::protocol;

typedef image::WorkUnitId id_type;
typedef image::WorkUnitData data_type;
typedef uint64_t metric_type;
struct Job {
    Job() :
                    bEmpty(true) {
    }

    Job(const PlaylistHelper &helper, sequence::Range overRange, size_t frame, EPlaybackState state) :
                    playlistIterator(helper, state, frame, overRange), bEmpty(helper.empty()) {
    }

    inline void clear() {
        bEmpty = true;
    }

    inline bool empty() const {
        return bEmpty || playlistIterator.empty();
    }

    inline id_type next() {
        assert(!empty());
        const id_type id = playlistIterator.front();
        playlistIterator.popFront();
        return id;
    }
private:
    CacheIterator playlistIterator;
    bool bEmpty;
};

typedef LookAheadCache<id_type, metric_type, data_type, Job> CACHE;
typedef concurrent::ConcurrentQueue<data_type> QUEUE;

static inline void checkValidAndPush(CACHE &jobProducer, const metric_type &weight, const data_type &unit) {
    const string &error = unit.imageHolder.error;
    if (!error.empty())
        cerr << "error while loading '" << unit.id.filename << "' : " << error << endl;
    jobProducer.put(unit.id, weight == 0 ? 1 : weight, unit);
}

static inline void decodeAndPush(const ImageDecoderFactory& factory, CACHE &jobProducer, data_type &unit) {
    metric_type weight;
    image::decode(factory, unit, weight);
    checkValidAndPush(jobProducer, weight, unit);
}

static inline void waitLoadAndPush(const ImageDecoderFactory& factory, CACHE &jobProducer, QUEUE &decodeQueue) {
    id_type id;
    jobProducer.waitAndPop(id);
    metric_type weight;
    data_type data(id);
    const bool isReady = image::load(factory, data, weight);
    if (isReady)
        checkValidAndPush(jobProducer, weight, data);
    else
        decodeQueue.push(data);
}

static void worker(const ImageDecoderFactory& factory, CACHE &jobProducer, QUEUE &decodeQueue) {
    data_type data;
    try {
        while (true) {
            if (decodeQueue.tryPop(data))
                decodeAndPush(factory, jobProducer, data);
            else
                waitLoadAndPush(factory, jobProducer, decodeQueue);
        }
    } catch (concurrent::terminated &e) {
    }
    while (decodeQueue.tryPop(data))
        decodeAndPush(factory, jobProducer, data);
}

static const string HEADER = "[Cache] ";

struct SmartCache::Impl : private boost::noncopyable {
    Impl(const duke::protocol::Cache& configuration, const ImageDecoderFactory& factory) :
                    m_ImageFactory(factory), m_Configuration(configuration), m_LookAheadCache(m_Configuration.size()) {
        launchWorkers();
    }

    ~Impl() {
        killWorkers();
    }

    inline void seek(const unsigned int frame, const EPlaybackState state) {
        m_LastJob = Job(playlist, cacheOverRange, frame, state);
        m_LookAheadCache.pushJob(m_LastJob);
    }

    inline void init(const PlaylistHelper &playlistHelper, const Cache &cache) {
        killWorkers();
        playlist = playlistHelper;
        m_Configuration.CopyFrom(cache);
        if (cache.has_size())
            m_LookAheadCache.setCacheSize(cache.size());
        if (cache.has_region()) {
            Range cacheRange(cache.region().first(), cache.region().last());
            if (playlist.range.contains(cacheRange.first) && playlist.range.contains(cacheRange.last)) {
                cacheOverRange = cacheRange;
            } else {
                cerr << HEADER + "invalid range for cache" << endl;
            }
        } else {
            cacheOverRange = playlist.range;
        }
        launchWorkers();
    }

    inline bool get(const MediaFrame &frame, ImageHolder & imageHolder) const {
        image::WorkUnitData data(frame);
        bool loaded = false;
        if (enabled() && m_LookAheadCache.get(frame, data))
            loaded = true;

        if (!loaded) {
            metric_type weight;
            if (!image::load(m_ImageFactory, data, weight))
                image::decode(m_ImageFactory, data, weight);
        }

        imageHolder = data.imageHolder;
        return imageHolder.error.empty();
    }

    inline metric_type dumpKeys(std::vector<image::WorkUnitId>& ids) const {
        return m_LookAheadCache.dumpKeys(ids);
    }

    bool enabled() const {
        return m_Configuration.threading() > 0 && m_Configuration.size() > 0;
    }

    const duke::protocol::Cache& configuration() const {
        return m_Configuration;
    }
private:
    void killWorkers() {
        m_LookAheadCache.terminate(true);
        m_ThreadGroup.join_all();
    }

    void launchWorkers() {
        if (!enabled())
            return;
        m_LookAheadCache.terminate(false);
        using boost::bind;
        for (uint32_t i = 0; i < m_Configuration.threading(); ++i)
            m_ThreadGroup.create_thread(bind(&::worker, boost::cref(m_ImageFactory), boost::ref(m_LookAheadCache), boost::ref(m_LoadedQueue)));
    }

    const ImageDecoderFactory& m_ImageFactory;
    ::duke::protocol::Cache m_Configuration;
    QUEUE m_LoadedQueue;
    CACHE m_LookAheadCache;
    PlaylistHelper playlist;
    sequence::Range cacheOverRange;
    Job m_LastJob;
    boost::thread_group m_ThreadGroup;
};

SmartCache::SmartCache(const duke::protocol::Cache& cacheConfiguration, const ImageDecoderFactory& factory) :
                m_pImpl(new SmartCache::Impl(cacheConfiguration, factory)) {
}

SmartCache::~SmartCache() {
}

void SmartCache::init(const duke::protocol::PlaylistHelper &playlist, const duke::protocol::Cache& cache) {
    m_pImpl->init(playlist, cache);
}

bool SmartCache::get(const MediaFrame &mf, ImageHolder & imageHolder) const {
    return m_pImpl->get(mf, imageHolder);
}

void SmartCache::seek(unsigned int frame, EPlaybackState state) {
    m_pImpl->seek(frame, state);
}

metric_type SmartCache::dumpKeys(image::WorkUnitIds &ids) const {
    return m_pImpl->dumpKeys(ids);
}

bool SmartCache::enabled() const {
    return m_pImpl->enabled();
}

const duke::protocol::Cache& SmartCache::configuration() const {
    return m_pImpl->configuration();
}
