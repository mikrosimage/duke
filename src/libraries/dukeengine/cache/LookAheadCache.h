/*
 * LookAheadCache.h
 *
 *  Created on: 9 nov. 2011
 *      Author: Guillaume Chatelet
 */

#ifndef LOOKAHEADCACHE_H_
#define LOOKAHEADCACHE_H_

#include "CacheKey.hpp"
#include "Cache.hpp"

#include <boost/thread.hpp>

namespace cache {

template<typename CACHE_ID_TYPE, typename DATA_TYPE, typename JOB_ID_TYPE>
class LookAheadCache : public Cache<CACHE_ID_TYPE, DATA_TYPE, JOB_ID_TYPE> {
private:
    typedef Cache<CACHE_ID_TYPE, DATA_TYPE, JOB_ID_TYPE> UP;
    typedef LookAheadCache<CACHE_ID_TYPE, DATA_TYPE, JOB_ID_TYPE> ME;
    boost::thread_group m_Threads;
protected:
    typedef typename UP::WorkUnit WorkUnit;
    typedef typename UP::WorkQueue WorkQueue;
    typedef typename UP::Map Map;

    /**
     * Inherited from Cache, see definition
     */
    virtual void limit(Map &cache) = 0;

    /**
     * The workerThread entry point.
     * You must implement this method and feed the UP::m_ReadyQueue
     * with completed jobs
     */
    virtual void workerThreadEntry() = 0;

public:
    virtual ~LookAheadCache() {
        m_Threads.join_all();
    }

    void start(uint8_t count) {
        assert(count!=0);
        for (; count != 0; --count)
            m_Threads.create_thread(boost::bind(&ME::workerThreadEntry, this));
    }

    inline bool isActive() const {
        return m_Threads.size() != 0;
    }
};

} // namespace cache

#endif /* LOOKAHEADCACHE_H_ */
