/*
 * CacheKey.hpp
 *
 *  Created on: 8 nov. 2011
 *      Author: Guillaume Chatelet
 */

#ifndef CACHEKEY_HPP_
#define CACHEKEY_HPP_

#include <cstddef>
#include <ostream>

namespace cache {

struct CacheKey {
    std::size_t jobId;
    std::size_t index;

    CacheKey(std::size_t jobId, std::size_t index) :
        jobId(jobId), index(index) {
    }

    bool operator<(const CacheKey& other) const {
        if (jobId == other.jobId)
            return index < other .index;
        return jobId > other.jobId;
    }

    inline bool operator==(const CacheKey& other) const {
        return index == other.index && jobId == other.jobId;
    }

    inline bool operator!=(const CacheKey& other) const {
        return !operator ==(other);
    }
};

std::ostream& operator<<(std::ostream& stream, const CacheKey& key) {
    stream << "Key(job=" << key.jobId << ",index=" << key.index << ")";
    return stream;
}

}  // namespace cache

#endif /* CACHEKEY_HPP_ */
