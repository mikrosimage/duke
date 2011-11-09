/*
 * Cache.hpp
 *
 *  Created on: 8 nov. 2011
 *      Author: Guillaume Chatelet
 */

#ifndef CACHE_HPP_
#define CACHE_HPP_

#include <dukeapi/io/ConcurrentQueue.h>

#include <map>
#include <set>
#include <vector>

namespace cache {

template<typename CACHE_ID_TYPE, typename DATA_TYPE, typename JOB_ID_TYPE>
class Cache {
public:
    typedef JOB_ID_TYPE jobid_type;
    typedef CACHE_ID_TYPE cacheid_type;
    typedef DATA_TYPE data_type;
    struct WorkUnit {
        jobid_type jobId;
        cacheid_type cacheId;
        data_type data;
        WorkUnit(const cacheid_type &id, const jobid_type &jobId) :
            jobId(jobId), cacheId(id) {
        }
        WorkUnit(const cacheid_type &id, const jobid_type &jobId, const data_type& data) :
            jobId(jobId), cacheId(id), data(data) {
        }
        inline bool operator==(const WorkUnit &other) const {
            return cacheId == other.cacheId && jobId == other.jobId;
        }
        inline bool operator!=(const WorkUnit &other) const {
            return !operator ==(other);
        }
        static bool keyOrdering(const WorkUnit &a, const WorkUnit &b) {
            return a.jobId < b.jobId;
        }
        static bool idOrdering(const WorkUnit &a, const WorkUnit &b) {
            return a.cacheId < b.cacheId;
        }
    };

    typedef ConcurrentQueue<WorkUnit> WorkQueue;
protected:
    typedef std::map<cacheid_type, WorkUnit> Map;
    typedef typename Map::iterator MapItr;
    typedef std::vector<WorkUnit> WorkUnits;
    typedef typename WorkUnits::const_iterator WorkUnitsConsItr;
    WorkQueue m_ReadyQueue;
private:
    WorkUnits m_DrainTemp;

    Map m_Cache;

    /**
     * Ask for insertion of a workunit
     * returns whether a unit was inserted
     */
    bool insert(const WorkUnit& unit) {
        const cacheid_type id(unit.cacheId);
        // unit already in cache ?
        const MapItr lower_bound(m_Cache.lower_bound(id));
        if (lower_bound == m_Cache.end()) { // map is empty, inserting
            m_Cache.insert(std::make_pair(id, unit));
        } else if (lower_bound->first != id) { // we're ready to insert from lower_bound
            m_Cache.insert(lower_bound, std::make_pair(id, unit));
        } else { // unit already set, updating only if jobId is newer
            const bool isKeyOlder = lower_bound->second.jobId < unit.jobId;
            if (isKeyOlder)
                return false;
            lower_bound->second = unit;
        }
        return true;
    }

    /**
     * Get all the available jobs from the queue at once and inserts them in the cache
     */
    bool drainAvailableJobs() {
        m_DrainTemp.clear();
        m_ReadyQueue.drainTo(m_DrainTemp);
        bool mapUpdated = false;
        const WorkUnitsConsItr end = m_DrainTemp.end();
        for (WorkUnitsConsItr itr = m_DrainTemp.begin(); itr != end; ++itr)
            mapUpdated |= insert(*itr);
        return mapUpdated;
    }

    /**
     * Ask for a clean of the map
     */
    void cleanup() {
        if (limit(m_Cache))
            cacheFull();
    }

public:
    virtual ~Cache() {
    }

    bool get(cacheid_type id, data_type &data) {
        if (drainAvailableJobs())
            cleanup();
        const typename Map::const_iterator found(m_Cache.find(id));
        if (found == m_Cache.end())
            return false;
        data = found->second.data;
        return true;
    }

    inline void manual_push(const WorkUnit& unit) {
        m_ReadyQueue.push(unit);
    }
protected:
    virtual bool limit(Map &cache) = 0;

    virtual void cacheFull() = 0;
};

} // namespace cache

#endif /* CACHE_HPP_ */
