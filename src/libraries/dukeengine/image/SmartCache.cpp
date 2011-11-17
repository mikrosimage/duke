/*
 * SmartCache.cpp
 *
 *  Created on: 9 nov. 2011
 *      Author: Guillaume Chatelet
 */

#include "SmartCache.h"

#include "ImageHolder.h"
#include "PlaylistIterator.h"

#include <dukeengine/cache/JobPriority.hpp>
#include <dukeengine/cache/Cache.hpp>

#include <dukeengine/host/io/ImageDecoderFactory.h>
#include <dukeengine/sequence/PlaylistHelper.h>

#include <boost/thread.hpp>

#include <cassert>

using namespace std;
using namespace cache;

struct WorkUnitData {
    string filename;
    ImageHolder imageHolder;
};

typedef uint64_t id_type;
typedef WorkUnitData data_type;
typedef JobPriority job_type;
typedef uint64_t metric_type;

typedef WorkUnit<data_type, id_type, job_type, metric_type> WORK_UNIT;

inline static void decode(WORK_UNIT&unit) {
    // decode unit and send to ready queue
    ostringstream msg;
    msg << "thread " << boost::this_thread::get_id() << " : " << unit.id << " READY !";
    printf("%s\n", msg.str().c_str());
}

inline static bool load(WORK_UNIT&unit) {
    // load unit and send to ready or loaded queue
    ostringstream msg;
    msg << "thread " << boost::this_thread::get_id() << " is loading " << unit.id;
    printf("%s\n", msg.str().c_str());
    return false;
}

struct Job {
    Job() :
            m_PlaylistIterator(PlaylistHelper(), 0, 0), m_JobKey(0, 0), m_Limited(true) {
    }

    Job(const PlaylistIterator playlistItr, size_t job) :
            m_PlaylistIterator(playlistItr), m_JobKey(job, 0), m_Limited(false) {
    }

    inline void clear() {
        m_Limited = true;
    }

    inline bool empty() const {
        return m_Limited || m_PlaylistIterator.empty();
    }

    inline WORK_UNIT next() {
        WORK_UNIT wu(front());
        popFront();
        return wu;
    }
private:
    void popFront() {
        m_PlaylistIterator.popFront();
        ++m_JobKey.index;
    }

    WORK_UNIT front() {
        WORK_UNIT wu(m_JobKey.index, m_JobKey);
        wu.id = m_PlaylistIterator.front();
        wu.data.filename = m_PlaylistIterator.frontFilename();
        return wu;
    }

    PlaylistIterator m_PlaylistIterator;
    JobPriority m_JobKey;bool m_Limited;
};

typedef JobProducer<WORK_UNIT, Job> CACHE;
typedef ConcurrentQueue<WORK_UNIT> QUEUE;


static void worker(CACHE &jobProducer, QUEUE &decodeQueue) {
    WORK_UNIT unit;
    try {
        while (true) {
            if (decodeQueue.tryPop(unit)) {
                decode(unit);
                jobProducer.push(unit);
            } else {
                jobProducer.waitAndPop(unit);
                if (load(unit))
                    jobProducer.push(unit);
                else
                    decodeQueue.push(unit);
            }
        }
    } catch (chain_terminated &e) {
    }
    while (decodeQueue.tryPop(unit)) {
        decode(unit);
        jobProducer.push(unit);
    }
}

struct SmartCache::Impl {
    Impl(const size_t threads, const uint64_t limit, const ImageDecoderFactory& factory) :
            m_ImageFactory(factory), m_LoadedQueue(), m_JobProducer(limit), m_CurrentJob(0) {
        using boost::bind;
        for (size_t i = 0; i < threads; ++i)
            m_ThreadGroup.create_thread(bind(&::worker, boost::ref(m_JobProducer), boost::ref(m_LoadedQueue)));
    }

    ~Impl() {
        m_JobProducer.terminate();
        m_ThreadGroup.join_all();
    }

    inline void seek(size_t frame, uint32_t speed, const PlaylistHelper &helper) {
        m_JobProducer.pushJob(Job(PlaylistIterator(helper, frame, speed), m_CurrentJob++));
    }

    inline bool get(uint64_t hash, ImageHolder & imageHolder) {
        WORK_UNIT tmp;
        if (!m_JobProducer.get(hash, tmp))
            return false;
        imageHolder = tmp.data.imageHolder;
        return true;
    }
private:

    const ImageDecoderFactory& m_ImageFactory;
    QUEUE m_LoadedQueue;
    CACHE m_JobProducer;
    size_t m_CurrentJob;
    boost::thread_group m_ThreadGroup;
};

SmartCache::SmartCache(uint8_t threads, uint64_t limit, const ImageDecoderFactory& factory) :
        m_pImpl(new Impl(threads, limit, factory)) {
}

bool SmartCache::get(uint64_t hash, ImageHolder & imageHolder) const {
    return m_pImpl->get(hash, imageHolder);
}

void SmartCache::seek(size_t frame, uint32_t speed, const PlaylistHelper &helper) {
    m_pImpl->seek(frame, speed, helper);
}

