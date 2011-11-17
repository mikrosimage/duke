/*
 * Cache.h
 *
 *  Created on: 16 nov. 2011
 *      Author: gch
 */

#ifndef CACHE_H_
#define CACHE_H_

#include <dukeapi/io/ConcurrentQueue.h>

#include "boost/concept/requires.hpp"
#include "boost/concept_check.hpp"

#include <map>
#include <iostream>
#include <atomic>

namespace cache {

template<typename DATA_TYPE, typename ID_TYPE, typename PRIORITY_TYPE, typename METRIC_TYPE>
struct WorkUnit {
    typedef DATA_TYPE data_type;
    typedef ID_TYPE id_type;
    typedef PRIORITY_TYPE priority_type;
    typedef METRIC_TYPE metric_type;

    BOOST_CONCEPT_ASSERT((boost::Comparable<id_type>)); //
    BOOST_CONCEPT_ASSERT((boost::UnsignedInteger<metric_type>)); //
    BOOST_CONCEPT_ASSERT((boost::Assignable<priority_type>)); //
    BOOST_CONCEPT_ASSERT((boost::LessThanComparable<priority_type>)); //

    WorkUnit(id_type id = 0, priority_type priority = priority_type(), metric_type metric = 0) :
            id(id), priority(priority), metric(metric) {
    }

    id_type id;
    priority_type priority;
    metric_type metric;
    data_type data;
};

enum UpdateState {
    DONE, REQUIRED, CACHE_FULL
};

template<typename WORK_UNIT>
struct Cache {
    typedef WORK_UNIT WorkUnit;
    typedef typename WORK_UNIT::id_type id_type;
    typedef typename WORK_UNIT::priority_type priority_type;
    typedef typename WORK_UNIT::metric_type metric_type;
    typedef std::map<priority_type, WorkUnit> Map;

    Cache(metric_type metricLimit) :
            m_MetricLimit(metricLimit), m_CurrentMetric(0) {
    }

    bool get(const id_type id, WorkUnit &unit) const {
        boost::mutex::scoped_lock lock(m_Mutex);
        const auto itr = findId(id);
        if (itr == m_Map.end())
            return false;
        unit = itr->second;
        return true;
    }

    Map dump() const {
        boost::mutex::scoped_lock lock(m_Mutex);
        return m_Map;
    }

    bool push(const WorkUnit &unit) {
        boost::mutex::scoped_lock lock(m_Mutex);
        if (m_MetricLimit < unit.metric) // won't fit in container anyway
            return false;

        if (beyongCacheLimit(unit))
            return false;

        m_CurrentMetric = makeRoom(m_MetricLimit - unit.metric);
        m_Map.insert(std::make_pair(unit.priority, unit));
        m_CurrentMetric += unit.metric;
        return true;
    }

    UpdateState query(const WorkUnit& unit) {
        boost::mutex::scoped_lock lock(m_Mutex);
        if (beyongCacheLimit(unit))
            return CACHE_FULL;
        const auto itr = findId(unit.id);
        if (itr == m_Map.end())
            return REQUIRED;
        if (unit.priority < itr->first) {
            m_Map.erase(itr);
            itr->second.priority = unit.priority;
            m_Map.insert(std::make_pair(unit.priority, itr->second));
        }
        return DONE;
    }

private:
    metric_type makeRoom(const metric_type keepMetricUnder) {
        metric_type sum(0);

        const auto itr = std::find_if(m_Map.begin(), m_Map.end(), [keepMetricUnder,&sum](const typename Map::value_type &pair) {
            sum += pair.second.metric;
            return sum > keepMetricUnder;
        });

        m_Map.erase(itr, m_Map.end());

        return sum;
    }

    inline typename Map::iterator findId(const id_type &id) {
        return std::find_if(m_Map.begin(), m_Map.end(), [id](const typename Map::value_type &pair) {return pair.second.id==id;});
    }

    inline typename Map::const_iterator findId(const id_type &id) const {
        return std::find_if(m_Map.begin(), m_Map.end(), [id](const typename Map::value_type &pair) {return pair.second.id==id;});
    }

    inline bool beyongCacheLimit(const WorkUnit&unit) const {
        if (m_Map.empty())
            return false; // always accept first element
        const bool lowerPriority = m_Map.rbegin()->first < unit.priority;
        const bool canFitInCache = m_CurrentMetric + unit.metric <= m_MetricLimit;
        return lowerPriority && !canFitInCache;
    }

    Map m_Map;
    mutable boost::mutex m_Mutex;
    const metric_type m_MetricLimit;
    metric_type m_CurrentMetric;
};

struct chain_terminated : public std::exception {
};

template<typename WORK_UNIT, typename WORK_UNIT_ONE_PASS_RANGE>
struct JobProducer : public Cache<WORK_UNIT> {
    typedef WORK_UNIT_ONE_PASS_RANGE WorkUnitRange;
    typedef Cache<WORK_UNIT> UP;
    typedef typename UP::WorkUnit WorkUnit;
    typedef typename UP::metric_type metric_type;

    BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<WorkUnitRange>));

    JobProducer(const metric_type cache_limit) :
            UP(cache_limit), m_Terminate(false) {
    }

    void waitAndPop(WorkUnit &unit) {
        do {
            checkTermination();
            unit = getJob();
            switch (UP::query(unit)) {
                case DONE:
                    break; // getting next one
                case REQUIRED:
                    return; // processing this
                case CACHE_FULL: {
                    boost::mutex::scoped_lock lock(m_Mutex);
                    m_SharedWorkUnitItr.clear();
                    break; // emptying queue and waiting for next job
                }
            }
        } while (true);
    }

    void pushJob(const WorkUnitRange &job) {
        jobQueue.push(job);
    }

    void terminate() {
        m_Terminate = true;
        pushJob(WorkUnitRange());
    }

private:
    inline WorkUnit getJob() {
        boost::mutex::scoped_lock lock(m_Mutex);
        checkTermination();

        // emptying the queue
        while (jobQueue.tryPop(m_SharedWorkUnitItr))
            ;

        // if current is empty waiting for a new one
        while (m_SharedWorkUnitItr.empty()) {
            jobQueue.waitPop(m_SharedWorkUnitItr);
            checkTermination();
        }

        // getting the unit
        return m_SharedWorkUnitItr.next();
    }

    inline void checkTermination() {
        if (m_Terminate)
            throw chain_terminated();
    }

    std::atomic<bool> m_Terminate;
    ConcurrentQueue<WorkUnitRange> jobQueue;
    boost::mutex m_Mutex;
    WorkUnitRange m_SharedWorkUnitItr;
};

} // namespace cache

#endif /* CACHE_H_ */
