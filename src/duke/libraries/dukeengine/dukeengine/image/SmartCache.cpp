/*
 * SmartCache.cpp
 *
 *  Created on: 9 nov. 2011
 *      Author: Guillaume Chatelet
 */

#include "SmartCache.h"

#include "ImageHolder.h"
#include "ImageToolbox.h"

#include <dukeengine/cache/LookAheadCache.hpp>
#include <dukeengine/host/io/ImageDecoderFactory.h>
#include <dukeengine/utils/CacheIterator.h>

#include <concurrent/ConcurrentQueue.h>
#include <dukeapi/sequence/PlaylistHelper.h>

#include <boost/thread.hpp>

#include <set>
#include <iterator>
#include <iostream>

#include <cassert>

using namespace std;
using namespace cache;
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
typedef ConcurrentQueue<data_type> QUEUE;

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
    } catch (terminated &e) {
    }
    while (decodeQueue.tryPop(data))
        decodeAndPush(factory, jobProducer, data);
}

static const string HEADER = "[Cache] ";

struct SmartCache::Impl : private boost::noncopyable {
    Impl(const size_t threads, const uint64_t limit, const ImageDecoderFactory& factory) :
                    m_CacheActivated(limit > 0 && threads > 0), m_ImageFactory(factory), m_LookAheadCache(limit) {

        if (threads == 0) {
            cerr << HEADER + "cache disabled, because threads = 0" << endl;
            return;
        }

        if (!m_CacheActivated) {
            cout << HEADER + "disabled" << endl;
            return;
        }

        cout << HEADER << (limit / 1024 / 1024) << "MiB with " << threads << " threads" << endl;
        using boost::bind;
        for (uint8_t i = 0; i < threads; ++i)
            m_ThreadGroup.create_thread(bind(&::worker, boost::cref(m_ImageFactory), boost::ref(m_LookAheadCache), boost::ref(m_LoadedQueue)));
    }

    ~Impl() {
        m_LookAheadCache.terminate();
        m_ThreadGroup.join_all();
    }

    inline void seek(const unsigned int frame, const EPlaybackState state) {
        m_LastJob = Job(playlist, cacheOverRange, frame, state);
        m_LookAheadCache.pushJob(m_LastJob);
    }

    inline bool get(const MediaFrame &frame, ImageHolder & imageHolder) const {
        image::WorkUnitData data(frame);
        bool loaded = false;
        if (m_CacheActivated) {
            if (m_LookAheadCache.get(frame, data)) {
                loaded = true;
            } else {
                cout << "image not in cache, loading in main thread" << endl;
            }
        }

        if (!loaded) {
            metric_type weight;
            if (!image::load(m_ImageFactory, data, weight))
                image::decode(m_ImageFactory, data, weight);
        }

        imageHolder = data.imageHolder;
        return imageHolder.error.empty();
    }

    inline void init(const duke::protocol::PlaylistHelper &playlistHelper, const sequence::Range &overRange) {
        playlist = playlistHelper;
        cacheOverRange = overRange;
    }

    inline void dumpKeys(std::vector<image::WorkUnitId>& ids) const {
        m_LookAheadCache.dumpKeys(ids);
    }

private:
    const bool m_CacheActivated;
    const ImageDecoderFactory& m_ImageFactory;
    QUEUE m_LoadedQueue;
    CACHE m_LookAheadCache;
    PlaylistHelper playlist;
    sequence::Range cacheOverRange;
    Job m_LastJob;
    boost::thread_group m_ThreadGroup;
};

SmartCache::SmartCache(size_t threads, uint64_t limit, const ImageDecoderFactory& factory) :
                m_pImpl(new SmartCache::Impl(threads, limit, factory)) {
}

SmartCache::~SmartCache() {
}

void SmartCache::init(const duke::protocol::PlaylistHelper &playlist, const sequence::Range &over) {
    m_pImpl->init(playlist, over);
}

bool SmartCache::get(const MediaFrame &mf, ImageHolder & imageHolder) const {
    return m_pImpl->get(mf, imageHolder);
}

void SmartCache::seek(unsigned int frame, EPlaybackState state) {
    m_pImpl->seek(frame, state);
}

void SmartCache::dumpKeys(image::WorkUnitIds &ids) const {
    m_pImpl->dumpKeys(ids);
}
