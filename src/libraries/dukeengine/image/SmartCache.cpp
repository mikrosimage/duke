/*
 * SmartCache.cpp
 *
 *  Created on: 9 nov. 2011
 *      Author: Guillaume Chatelet
 */

#include "SmartCache.h"

#include "ImageHolder.h"
#include "PlaylistIterator.h"

#include <dukeengine/cache/CacheKey.hpp>
#include <dukeengine/cache/LookAheadCache.h>

#include <dukeengine/host/io/ImageDecoderFactory.h>
#include <dukeengine/sequence/PlaylistHelper.h>

#include <cassert>

using namespace std;
using namespace cache;

struct cache_terminated : public std::exception {
};

struct Job {
    size_t frame;
    uint32_t speed;
    bool terminate;
    PlaylistHelper helper;
};

struct SmartCacheImpl : public LookAheadCache<uint64_t, ImageHolder, CacheKey> {
    SmartCacheImpl(uint64_t limit, const ImageDecoderFactory& factory) :
        m_CurrentItr(PlaylistHelper(), 0, 0), m_CurrentJobIndex(0), m_CacheSizeLimit(limit), m_ImageDecoderFactory(factory), m_Initialized(false), m_Terminate(false) {
    }
    ~SmartCacheImpl() {
        m_JobQueue.push(Job { -1, -1, true }); // pushing a quit message
    }

    inline void seek(size_t frame, uint32_t speed, const PlaylistHelper &helper) {
        m_JobQueue.push(Job { frame, speed, false, helper });
    }

private:
    typedef LookAheadCache<uint64_t, ImageHolder, CacheKey> UP;
    typedef typename UP::WorkUnit WorkUnit;

    virtual void limit(Map &cache) {
        if (cache.size() > 100) {
            ::boost::mutex::scoped_lock guard(m_Mutex);
            m_CurrentItr.limitHere();
        }
    }

    /**
     *
     */
    virtual void workerThreadEntry() {
        WorkUnit unit(-1, CacheKey(-1, -1)); // default initialization
        try {
            while (true) {
                if (m_LoadedQueue.tryPop(unit)) {
                    decode(unit);
                } else {
                    const bool queueEmpty(!m_IncomingQueue.tryPop(unit));
                    if (queueEmpty) {
                        askForMoreWork();
                        m_IncomingQueue.waitPop(unit);
                    }
                    load(unit);
                }
            }
        } catch (cache_terminated &e) {
        }
    }

    void askForMoreWork() {
        ::boost::mutex::scoped_lock guard(m_Mutex);
        if (m_Terminate)
            throw cache_terminated();
        // testing if new job are present
        vector<Job> pendingJobs;
        m_JobQueue.drainTo(pendingJobs);
        if (!pendingJobs.empty()) {
            if (pendingJobs.back().terminate) {
                m_Terminate = true;
                throw cache_terminated();
            }
            setNewJob(pendingJobs.back());
        }
        // if no job there or previous one finished, waiting for one
        if (!m_Initialized || m_CurrentItr.empty()) {
            Job job;
            m_JobQueue.waitPop(job);
            setNewJob(job);
        }
        assert(m_Initialized); // here we are initialized
        // a job is present and not finished, adding some more work to the queue
        for (size_t toAdd = 20; toAdd != 0 && !m_CurrentItr.empty(); --toAdd, m_CurrentItr.popFront()) {
            m_IncomingQueue.push(WorkUnit(m_CurrentItr.front(), CacheKey(m_CurrentJobIndex, m_CurrentItr.index())));
            ostringstream msg;
            msg << "pushing new work unit " << m_CurrentItr.front() << " with index " << m_CurrentItr.index();
            printf("%s\n", msg.str().c_str());
        }
    }

    void setNewJob(const Job& job) {
        // setting the new job
        ++m_CurrentJobIndex;
        m_CurrentItr = IndexedPlaylistIterator(job.helper, job.frame, job.speed);
        m_Initialized = true;
        ostringstream msg;
        msg << "added new job " << m_CurrentJobIndex;
        printf("%s\n", msg.str().c_str());
        // cleaning queues
        m_IncomingQueue.clear();
    }

    void decode(WorkUnit unit) {
        // decode unit and send to ready queue
        ostringstream msg;
        msg << "thread " << boost::this_thread::get_id() << " : " << unit.cacheId << " READY !";
        printf("%s\n", msg.str().c_str());
        m_ReadyQueue.push(unit);
    }

    void load(WorkUnit unit) {
        // load unit and send to ready or loaded queue
        ostringstream msg;
        msg << "thread " << boost::this_thread::get_id() << " is loading " << unit.cacheId;
        printf("%s\n", msg.str().c_str());
        m_LoadedQueue.push(unit);
    }

private:
    ConcurrentQueue<WorkUnit> m_LoadedQueue;
    ConcurrentQueue<WorkUnit> m_IncomingQueue;
    ConcurrentQueue<Job> m_JobQueue;
    IndexedPlaylistIterator m_CurrentItr;
    size_t m_CurrentJobIndex;

    const uint64_t m_CacheSizeLimit;
    const ImageDecoderFactory& m_ImageDecoderFactory;
    bool m_Initialized;
    bool m_Terminate;
    ::boost::mutex m_Mutex;
};

SmartCache::SmartCache(uint64_t limit, const ImageDecoderFactory& factory) :
    m_pImpl(new SmartCacheImpl(limit, factory)) {
    m_pImpl->start(1);
}

bool SmartCache::get(uint64_t hash, ImageHolder & imageHolder) const {
    return m_pImpl->get(hash, imageHolder);
    //    const uint64_t hash = helper.getHashAtIterator(itrIndex);
    //    if (hash == 0)
    //        return false;
    //
    //    const boost::filesystem::path filename = helper.getPathAtHash(hash);
    //    if (filename.empty())
    //        return true;
    //
    //    return imageReader.load(filename, value);
    //    return false;
}

void SmartCache::seek(size_t frame, uint32_t speed, const PlaylistHelper &helper) {
    m_pImpl->seek(frame, speed, helper);
}

