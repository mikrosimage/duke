/*
 * LookAheadCache.hpp
 *
 *  Created on: 22 nov. 2011
 *      Author: Guillaume Chatelet
 */

#ifndef __LOOK_AHEAD_CACHE_HPP__
#define __LOOK_AHEAD_CACHE_HPP__

#include "PriorityCache.hpp"

#include <dukeapi/io/BlockingAccessor.hpp>

#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>

#include <iostream>
#include <map>
#include <set>
#include <cassert>

namespace cache {

template<typename ID_TYPE, typename METRIC_TYPE, typename DATA_TYPE, typename WORK_UNIT_RANGE>
struct LookAheadCache {
    typedef ID_TYPE id_type;
    typedef METRIC_TYPE metric_type;
    typedef DATA_TYPE data_type;
    typedef WORK_UNIT_RANGE WorkUnitRange;

    LookAheadCache(const metric_type cache_limit) :
        m_SharedCache(cache_limit), m_PendingJob(WorkUnitRange()) {
    }

    inline bool offer(const id_type &id, const metric_type weight, const data_type &data) {
        boost::mutex::scoped_lock lock(m_CacheMutex);
        if (m_SharedCache.contains(id)) {
            std::cout << "did some work for nothing, " << id << " was already in the cache" << std::endl;
            return false;
        }
        return m_SharedCache.put(id, weight, data);
    }

    inline bool get(const id_type &id, data_type &data, std::vector<id_type> &allKeys) const {
        boost::mutex::scoped_lock lock(m_CacheMutex);
        m_SharedCache.dumpKeys(allKeys);
        return m_SharedCache.get(id, data);
    }

    inline bool get(const id_type &id, data_type &data) const {
        boost::mutex::scoped_lock lock(m_CacheMutex);
        return m_SharedCache.get(id, data);
    }

    void waitAndPop(id_type &unit) {
        boost::mutex::scoped_lock lock(m_PopMutex);
        do {
            unit = nextWorkUnit();
            COUT("next unit is : " << unit.filename << std::endl);
            boost::mutex::scoped_lock lock(m_CacheMutex);
            switch (m_SharedCache.update(unit)) {
                case FULL:
                    COUT("cache is full, emptying current job" << std::endl);
                    m_SharedWorkUnitItr.clear();
                    break;
                case NOT_NEEDED:
                    COUT("unit updated, checking another one" << std::endl);
                    break;
                case NEEDED:
                    COUT("serving " << unit << std::endl);
                    return;
            }
        } while (true);
    }

    void pushJob(const WorkUnitRange &job) {
        m_PendingJob.set(job);
    }

    void terminate() {
        m_PendingJob.terminate();
    }

    mutable boost::mutex m_CacheMutex;
private:
    inline id_type nextWorkUnit() {
        if (updateJob()) {
            boost::mutex::scoped_lock lock(m_CacheMutex);
            m_SharedCache.discardPending();
//            std::cout << "received new Job" << std::endl;
        }
        return m_SharedWorkUnitItr.next();
    }

    inline bool updateJob() {
        bool updated = m_PendingJob.tryGet(m_SharedWorkUnitItr);
        while (m_SharedWorkUnitItr.empty()) {
            m_PendingJob.waitGet(m_SharedWorkUnitItr);
            updated = true;
        }
        return updated;
    }

    mutable boost::mutex m_PopMutex;
    PriorityCache<id_type, metric_type, data_type> m_SharedCache;
    BlockingAccessor<WorkUnitRange> m_PendingJob;
    WorkUnitRange m_SharedWorkUnitItr;
};

} // namespace cache

#endif /* __LOOK_AHEAD_CACHE_HPP__ */
