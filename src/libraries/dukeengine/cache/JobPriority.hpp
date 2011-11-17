/*
 * JobPriority.hpp
 *
 *  Created on: 8 nov. 2011
 *      Author: Guillaume Chatelet
 */

#ifndef JOBPRIORITY_HPP_
#define JOBPRIORITY_HPP_

#include <cstddef>
#include <ostream>

namespace cache {

struct JobPriority {
    std::size_t jobId;
    std::size_t index;

    JobPriority() :
            jobId(-1), index(-1) {
    }
    JobPriority(std::size_t jobId, std::size_t index) :
            jobId(jobId), index(index) {
    }

    bool operator<(const JobPriority& other) const {
        if (jobId == other.jobId)
            return index < other.index;
        return jobId > other.jobId;
    }

    inline bool operator==(const JobPriority& other) const {
        return index == other.index && jobId == other.jobId;
    }

    inline bool operator!=(const JobPriority& other) const {
        return !operator ==(other);
    }
};

std::ostream& operator<<(std::ostream& stream, const JobPriority& key) {
    stream << "Key(job=" << key.jobId << ",index=" << key.index << ")";
    return stream;
}

} // namespace cache

#endif /* JOBPRIORITY_HPP_ */
