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

#include <vector>
#include <deque>
#include <map>
#include <algorithm>
#include <stdexcept>
#include <cassert>

//#define DEBUG_CACHE

#ifdef DEBUG_CACHE
#include <iostream>
#define COUT(X) std::cout<<X
#else
#define COUT(X)
#endif

namespace cache {

enum UpdateStatus {
    FULL, NEEDED, NOT_NEEDED
};

template<typename ID_TYPE, typename METRIC_TYPE, typename DATA_TYPE>
struct PriorityCache : private boost::noncopyable {
    typedef ID_TYPE id_type;
    typedef METRIC_TYPE metric_type;
    typedef DATA_TYPE data_type;

    BOOST_CONCEPT_ASSERT((boost::LessThanComparable<id_type>)); // for map
    BOOST_CONCEPT_ASSERT((boost::EqualityComparable<id_type>)); //
    BOOST_CONCEPT_ASSERT((boost::UnsignedInteger<metric_type>)); //

private:
    typedef std::deque<id_type> IdContainer;

    struct DataElement {
        metric_type metric;
        data_type data;
    };
    typedef std::map<id_type, DataElement> CacheContainer;

public:
    PriorityCache(metric_type limit) :
        m_CacheLimit(limit) {
        COUT( "#############################################################################################################" << std::endl);
    }

    void dumpKeys(std::vector<id_type> &key_container) const {
        key_container.clear();
        key_container.reserve(m_Cache.size());
        for (const typename CacheContainer::value_type& pair : m_Cache)
            key_container.push_back(pair.first);
    }

    inline bool isFull() const {
        return m_CacheLimit == 0 || contiguousWeight() > m_CacheLimit;
    }

    inline bool contains(id_type id) const {
        return m_Cache.find(id) != m_Cache.end();
    }

    inline bool isPending(id_type id) const {
        return contains(m_PendingIds, id);
    }

    inline metric_type cacheSize() const {
        return currentWeight();
    }

    void discardPending() {
        std::copy(m_PendingIds.rbegin(), m_PendingIds.rend(), std::front_inserter(m_DiscardableIds));
        m_PendingIds.clear();
    }

    UpdateStatus update(id_type id) {
        if (isFull())
            return FULL; //
        COUT("Updating " << id << std::endl);
        const bool wasRequested = remove(id);
        m_PendingIds.push_back(id);
        const UpdateStatus status = wasRequested || contains(id) ? NOT_NEEDED : NEEDED;
        if (status == NEEDED)
            dump("update dump");
        return status;
    }

    bool put(const id_type &id, const metric_type weight, const data_type &data) {
        using namespace std;

        COUT("=============================================================================================================" << endl);
        if (weight == 0)
            throw logic_error("can't put an id with no weight");
        if (contains(id))
            throw logic_error("id is already present in cache");

        if (isFull()) {
            COUT("cache is *full*, discarding " << id << endl);
            remove(id); // no more pending
            dump("cache full dump");
            return false;
        }
        if (!canFit(weight)) {
            COUT("trying to make room for " << id << endl);
            makeRoomFor(id, weight);
        }
        if (isFull())
            return false;
        addToCache(id, weight, data);
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
    void dump(const char* dumpMessage) const {
        using namespace std;
#ifdef DEBUG_CACHE
        cout << dumpMessage << endl;
        cout << "pendings {" << endl;
        display(m_PendingIds);
        cout << "}" << endl;
        cout << "discardables {" << endl;
        display(m_DiscardableIds);
        cout << "}" << endl;
        cout << "-------------------------------------------------------------------------------------------------------------" << endl;
#endif
    }

#ifdef DEBUG_CACHE
    inline void display(const std::deque<id_type> &queue) const {
        using namespace std;
        for (const id_type &id : queue) {
            cout << id;
            if (!contains(id)) {
                cout << " [ ]";
            } else {
                cout << " [X]";
            }
            cout << endl;
        }
    }
#endif

    inline static bool contains(const IdContainer &container, const id_type &value) {
        return std::find(container.begin(), container.end(), value) != container.end();
    }

    inline static bool remove(IdContainer &container, const id_type &value) {
        auto itr = std::remove(container.begin(), container.end(), value);
        if (itr == container.end())
            return false;
        container.erase(itr, container.end());
        return true;
    }
//    inline static void remove(IdContainer &container, const id_type &value) {
//        container.erase(std::remove(container.begin(), container.end(), value), container.end());
//    }

    inline bool remove(const id_type &value) {
        return remove(m_PendingIds, value) || remove(m_DiscardableIds, value);
    }

    metric_type contiguousWeight() const {
        metric_type sum = 0;
        const auto &end = m_Cache.end();
        for (const id_type& id : m_PendingIds) {
            const auto &itr = m_Cache.find(id);
            if (itr == end)
                return sum;
            sum += itr->second.metric;
        }
        return sum;
    }

    metric_type currentWeight() const {
        metric_type sum = 0;
        for (const typename CacheContainer::value_type &pair : m_Cache)
            sum += pair.second.metric;
        return sum;
    }

    inline bool canFit(const metric_type weight) const {
        if (weight > m_CacheLimit)
            return false;
        const metric_type maxWeight = m_CacheLimit - weight;
        return currentWeight() <= maxWeight;
    }

    void makeRoomFor(const id_type currentId, const metric_type weight) {
        using namespace std;

        COUT("{ " << currentWeight() << endl);
        const metric_type maxWeight = m_CacheLimit - weight;
        IdContainer toEvict;
        copy(m_DiscardableIds.rbegin(), m_DiscardableIds.rend(), std::back_inserter(toEvict));
        copy(m_PendingIds.rbegin(), m_PendingIds.rend(), std::back_inserter(toEvict));
        for (const auto &id : toEvict) {
            if (currentWeight() == contiguousWeight())
                break;
            evict(id);
            if (currentWeight() <= maxWeight)
                break;
        }COUT("} " << currentWeight() << endl);
    }

    inline void evict(id_type id) {
        auto itr = m_Cache.find(id);
        if (itr == m_Cache.end())
            return; // not found
        m_Cache.erase(itr);
        remove(id);
        COUT("\t- " << id << std::endl);
    }

    void addToCache(const id_type &id, const metric_type weight, const data_type &data) {
        if (!(contains(m_PendingIds, id) || contains(m_DiscardableIds, id)))
            m_DiscardableIds.push_back(id);
        m_Cache.insert(std::make_pair(id, DataElement { weight, data }));
        COUT("+ " << id << std::endl);
    }

private:
    const metric_type m_CacheLimit;
    IdContainer m_DiscardableIds;
    IdContainer m_PendingIds;
    CacheContainer m_Cache;
};

} // namespace cache

#endif /* PRIORITYCACHE_HPP_ */
