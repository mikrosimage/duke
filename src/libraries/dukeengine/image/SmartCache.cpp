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

#include <iterator>

#include <cassert>
#include <cinttypes>

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
        const id_type id { m_PlaylistIterator.front(), m_PlaylistIterator.frontFilename() };
        m_PlaylistIterator.popFront();
        return id;
    }

    inline const PlaylistHelper& helper() const {
        return m_PlaylistIterator.helper();
    }

private:
    PlaylistIterator m_PlaylistIterator;bool m_Limited;
};

typedef LookAheadCache<id_type, metric_type, data_type, Job> CACHE;
typedef ConcurrentQueue<data_type> QUEUE;

static inline void checkValidAndPush(CACHE &jobProducer, const metric_type &weight, const data_type &unit) {
    if (!unit.error.empty())
        printf("error while loading '%s' : %s\n", unit.id.filename.c_str(), unit.error.c_str());
    jobProducer.offer(unit.id, weight == 0 ? 1 : weight, unit);
}

static inline void decodeAndPush(const ImageDecoderFactory& factory, CACHE &jobProducer, data_type &unit) {
    metric_type weight;
    image::decode(factory, unit, weight);
    checkValidAndPush(jobProducer, weight, unit);
}

static inline void waitLoadAndPush(const ImageDecoderFactory& factory, CACHE &jobProducer, QUEUE &decodeQueue) {
    id_type id;
    jobProducer.waitAndPop(id);
//    printf(">> popping job %s\n", id.filename.c_str());
    metric_type weight;
    data_type data(id);
    const bool isReady = image::load(factory, data, weight);
    if (isReady)
        checkValidAndPush(jobProducer, weight, data);
    else
        decodeQueue.push(data);
}

static void worker(const ImageDecoderFactory& factory, CACHE &jobProducer, QUEUE &decodeQueue) {
    printf(">> worker launched\n");
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
    printf(">> worker stopped\n");
}

struct SmartCache::Impl : private boost::noncopyable {
    Impl(const size_t threads, const uint64_t limit, const ImageDecoderFactory& factory) :
            m_ImageFactory(factory), m_LookAheadCache(limit), m_LastFrame(-1), m_LastSpeed(-1), m_pLastHelper(NULL) {
        using boost::bind;
        if (limit > 0)
            for (size_t i = 0; i < threads; ++i)
                m_ThreadGroup.create_thread(bind(&::worker, boost::cref(m_ImageFactory), boost::ref(m_LookAheadCache), boost::ref(m_LoadedQueue)));
    }

    ~Impl() {
        m_LookAheadCache.terminate();
        m_ThreadGroup.join_all();
    }

    inline void seek(const size_t frame, const uint32_t speed, const PlaylistHelper &helper) {
        if (frame == m_LastFrame && speed == m_LastSpeed && m_pLastHelper == &helper)
            return;
        m_LastFrame = frame;
        m_LastSpeed = speed;
        m_pLastHelper = &helper;
        m_LastJob = Job(helper, frame, speed);
//        cout << "posting new job ";
        m_LookAheadCache.pushJob(m_LastJob);
    }

    inline bool get(uint64_t hash, ImageHolder & imageHolder) const {
        const image::WorkUnitId id(hash, m_LastJob.helper().getPathStringAtHash(hash));
        if (id.filename.empty())
            return false;
        image::WorkUnitData data(id);
        if (!m_LookAheadCache.get(id, data)) {
            cout << "image not in cache, loading in main thread" << endl;
            metric_type weight;
            if (!image::load(m_ImageFactory, data, weight))
                image::decode(m_ImageFactory, data, weight);
        }
        dump();
        imageHolder = data.imageHolder;
        return data.error.empty();
    }

private:
    void dump() const {
        vector<id_type> ids;
        m_LookAheadCache.dumpAvailableKeys(ids);
//        copy(ids.begin(), ids.end(), ostream_iterator<id_type>(cout, " "));
        cout << ids[0] << endl;
    }

    const ImageDecoderFactory& m_ImageFactory;
    QUEUE m_LoadedQueue;
    CACHE m_LookAheadCache;
    Job m_LastJob;
    size_t m_LastFrame;
    uint32_t m_LastSpeed;
    const PlaylistHelper * m_pLastHelper;
    boost::thread_group m_ThreadGroup;
};

SmartCache::SmartCache(uint8_t threads, uint64_t limit, const ImageDecoderFactory& factory) :
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

