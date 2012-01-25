/*
 * SmartCache.cpp
 *
 *  Created on: 9 nov. 2011
 *      Author: Guillaume Chatelet
 */

#include "SmartCache.h"

#include "ImageHolder.h"
#include "PlaylistIterator.h"
#include "ImageToolbox.h"

#include <dukeapi/io/ConcurrentQueue.h>

#include <dukeengine/cache/LookAheadCache.hpp>
#include <dukeengine/host/io/ImageDecoderFactory.h>
#include <dukeengine/sequence/PlaylistHelper.h>

#include <boost/thread.hpp>

#include <set>
#include <iterator>
#include <iostream>

#include <cassert>

using namespace std;
using namespace cache;

typedef image::WorkUnitId id_type;
typedef image::WorkUnitData data_type;
typedef uint64_t metric_type;

struct Job {
    Job() :
        m_PlaylistIterator(PlaylistHelper(), 0, 0), m_Limited(true) {
    }

    Job(const PlaylistHelper &helper, size_t frame, uint32_t speed) :
        m_PlaylistIterator(helper, frame, speed), m_Limited(false) {
    }

    inline void clear() {
        m_Limited = true;
    }

    inline bool empty() const {
        return m_Limited || m_PlaylistIterator.empty();
    }

    inline id_type next() {
        const id_type id(m_PlaylistIterator.front(), m_PlaylistIterator.frontFilename());
        m_PlaylistIterator.popFront();
        return id;
    }

    inline const PlaylistHelper& helper() const {
        return m_PlaylistIterator.helper();
    }

private:
    PlaylistIterator m_PlaylistIterator;
    bool m_Limited;
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
    try {
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
    } catch (std::exception &e) {
        std::cerr << "[Cache] " << e.what() << std::endl;
    }
}

struct SmartCache::Impl : private boost::noncopyable {
    Impl(const size_t threads, const uint64_t limit, const ImageDecoderFactory& factory) :
        m_CacheActivated(limit > 0 && threads > 0), m_ImageFactory(factory), m_LookAheadCache(limit) {

        if (threads == 0) {
            cerr << "[Cache] cache disabled, because threads = 0" << endl;
            return;
        }

        if (!m_CacheActivated) {
            cout << "[Cache] disabled" << endl;
            return;
        }

        cout << "[Cache] " << (limit / 1024 / 1024) << "MiB with " << threads << " threads" << endl;
        using boost::bind;
        for (uint8_t i = 0; i < threads; ++i)
            m_ThreadGroup.create_thread(bind(&::worker, boost::cref(m_ImageFactory), boost::ref(m_LookAheadCache), boost::ref(m_LoadedQueue)));
    }

    ~Impl() {
        m_LookAheadCache.terminate();
        m_ThreadGroup.join_all();
    }

    inline void seek(const size_t frame, const uint32_t speed, const PlaylistHelper &helper) {
        m_LastJob = Job(helper, frame, speed);
        m_LookAheadCache.pushJob(m_LastJob);
    }

    inline bool get(uint64_t hash, ImageHolder & imageHolder) const {
        const PlaylistHelper &helper = m_LastJob.helper();
        const image::WorkUnitId id(hash, helper.getPathStringAtHash(hash));
        image::WorkUnitData data(id);
        bool loaded = false;
        if (m_CacheActivated) {
            displayQueueState(hash);
            if (m_LookAheadCache.get(id, data)) {
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

private:
    inline void displayQueueState(const uint64_t &currentHash) const {
        m_LookAheadCache.dumpKeys(m_AvailableKeys);
        const PlaylistHelper &helper = m_LastJob.helper();
        m_QueryKeys.clear();
        BOOST_FOREACH (const id_type& id, m_AvailableKeys)
                    {
                        m_QueryKeys.insert(id.hash);
                    }
        const set<uint64_t>::const_iterator end = m_QueryKeys.end();
        cout << '[';
        for (size_t itr = 0; itr < helper.getEndIterator(); ++itr) {
            const uint64_t hash = helper.getHashAtIterator(itr);
            if (hash == currentHash)
                cout << '#';
            else {
                if (m_QueryKeys.find(hash) == end)
                    cout << '_';
                else
                    cout << 'o';
            }
        }
        cout << "] " << m_QueryKeys.size() << '\r';
        cout.flush();
    }

    const bool m_CacheActivated;
    const ImageDecoderFactory& m_ImageFactory;
    QUEUE m_LoadedQueue;
    CACHE m_LookAheadCache;
    Job m_LastJob;
    //    size_t m_LastFrame;
    //    uint32_t m_LastSpeed;
    //    const PlaylistHelper * m_pLastHelper;
    boost::thread_group m_ThreadGroup;

    mutable vector<id_type> m_AvailableKeys;
    mutable set<uint64_t> m_QueryKeys;
};

SmartCache::SmartCache(size_t threads, uint64_t limit, const ImageDecoderFactory& factory) :
    m_pImpl(new SmartCache::Impl(threads, limit, factory)) {
}

SmartCache::~SmartCache() {
}

bool SmartCache::get(uint64_t hash, ImageHolder & imageHolder) const {
    return m_pImpl->get(hash, imageHolder);
}

void SmartCache::seek(size_t frame, uint32_t speed, const PlaylistHelper &helper) {
    m_pImpl->seek(frame, speed, helper);
}

