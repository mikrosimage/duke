/*
 * PriorityCache.hpp
 *
 *  Created on: 22 nov. 2011
 *      Author: Guillaume Chatelet
 */

#ifndef PRIORITYCACHE_HPP_
#define PRIORITYCACHE_HPP_

#include <boost/noncopyable.hpp>
#include <boost/concept_check.hpp>

#include <deque>
#include <map>
#include <algorithm>
#include <stdexcept>
#include <cassert>

namespace cache {

template<typename ID_TYPE, typename METRIC_TYPE, typename DATA_TYPE>
struct PriorityCache : private boost::noncopyable {
    typedef ID_TYPE id_type;
    typedef METRIC_TYPE metric_type;
    typedef DATA_TYPE data_type;

    BOOST_CONCEPT_ASSERT((boost::LessThanComparable<id_type>)); // for map
    BOOST_CONCEPT_ASSERT((boost::EqualityComparable<id_type>)); //
    BOOST_CONCEPT_ASSERT((boost::UnsignedInteger<metric_type>)); //

    PriorityCache(metric_type limit) :
            m_CurrentWeight(0), m_CacheLimit(limit) {
    }

    void dumpAvailableKeys(std::vector<id_type> &keys) const {
        keys.clear();
        std::for_each(m_Cache.begin(), m_Cache.end(), [&keys](const std::pair<id_type, DataElement> &pair){keys.push_back(pair.first);});
    }

    inline bool isFull() const {
        return m_CurrentWeight > m_CacheLimit || m_CacheLimit == 0;
    }

    /**
     * true if updated
     * false if first time request
     */
    bool update(id_type id) {
        const bool found = removeFrom(m_DiscardableIds, id) || removeFrom(m_PendingIds, id);
        m_PendingIds.push_back(id);
        return found;
    }

    inline bool contains(id_type id) const {
        return m_Cache.find(id) != m_Cache.end();
    }

    inline bool isPending(id_type id) const {
        return std::find(m_PendingIds.begin(), m_PendingIds.end(), id) != m_PendingIds.end();
    }

    inline metric_type cacheSize() const {
        return m_CurrentWeight;
    }

    void discardPending() {
        std::copy(m_PendingIds.rbegin(), m_PendingIds.rend(), std::front_inserter(m_DiscardableIds));
        m_PendingIds.clear();
    }

    bool put(const id_type &id, const metric_type weight, const data_type &data) {
        if (weight == 0)
            throw std::logic_error("can't put an id with no weight");
        if (contains(id))
            throw std::logic_error("id is already present in cache");

        if (isFull()) {
            makeRoomFor(id, weight); // trying to evict some
            if (isFull()) // still full
                return false;
        }
        if (canFit(weight)) { //can fit in cache
            addToCache(id, weight, data);
            if (!isPending(id) && !hasDiscardable(id)) // not requested so adding to discardable
                m_DiscardableIds.push_back(id);
        } else { //can't fit in cache
            if (!isPending(id)) // not requested and can't fit...canceling
                return false;
            // pending but not enough room
            makeRoomFor(id, weight);
            addToCache(id, weight, data);
        }
        return true;
    }

    bool get(const id_type &id, data_type &data) const {
        const auto itr = m_Cache.find(id);
        if (itr == m_Cache.end())
            return false;
        data = itr->second.data;
        return true;
    }
private:

    bool hasDiscardable(const id_type value) {
        return std::find(m_DiscardableIds.begin(), m_DiscardableIds.end(), value) != m_DiscardableIds.end();
    }

    static bool removeFrom(std::deque<id_type>& container, id_type value) {
        auto itr = std::find(container.begin(), container.end(), value);
        if (itr == container.end())
            return false;
        container.erase(itr);
        return true;
    }

    void makeRoomFor(const id_type currentId, const metric_type weight) {
        const metric_type maxWeight = m_CacheLimit - weight;
        keepCacheSizeBelow(m_DiscardableIds, maxWeight, id_type(), false);
        keepCacheSizeBelow(m_PendingIds, maxWeight, currentId, true);
    }

    void keepCacheSizeBelow(std::deque<id_type> &container, const metric_type maxSize, const id_type stopAtId, const bool checkStopValue) {
        while (m_CurrentWeight >= maxSize && !container.empty()) {
            auto idToDelete = container.back();
            if (checkStopValue && idToDelete == stopAtId)
                return;
            container.pop_back();
            removeFromCache(idToDelete);
        }
    }

    inline void removeFromCache(id_type id) {
        auto itr = m_Cache.find(id);
        if (itr == m_Cache.end())
            return; // not found
        m_CurrentWeight -= itr->second.metric;
        m_Cache.erase(itr);
    }

    void addToCache(const id_type &id, const metric_type weight, const data_type &data) {
        m_CurrentWeight += weight;
        m_Cache.insert(std::make_pair(id, DataElement { weight, data }));
    }

    inline bool canFit(const metric_type weight) const {
        if (weight > m_CacheLimit)
            return false;
        const metric_type maxWeight = m_CacheLimit - weight;
        return m_CurrentWeight <= maxWeight;
    }

private:
    struct DataElement {
        metric_type metric;
        data_type data;
    };

    std::deque<id_type> m_DiscardableIds;
    std::deque<id_type> m_PendingIds;
    std::map<id_type, DataElement> m_Cache;
    metric_type m_CurrentWeight;
    metric_type m_CacheLimit;
};

} // namespace cache

#endif /* PRIORITYCACHE_HPP_ */
