#include "FileBufferHolder.h"

#include <dukeengine/image/SmartCache.h>
#include <dukeengine/sequence/PlaylistHelper.h>
#include <dukeio/ImageDescription.h>

#include <boost/foreach.hpp>

#include <iostream>

using namespace std;

FileBufferHolder::FileBufferHolder() {
}

void FileBufferHolder::update(const std::size_t currentFrame, const SmartCache& cache, const PlaylistHelper& helper) {
    m_Images.clear();

    helper.getIteratorsAtFrame(currentFrame, m_Iterators);
    BOOST_FOREACH(const size_t itr, m_Iterators)
    {
        m_Images.push_back(ImageHolder()); // pushing an empty description
        const uint64_t hash = helper.getHashAtIterator(itr);
        if (!cache.get(hash, m_Images.back())) {
            const string &error = m_Images.back().error;
            std::cerr << "At frame " << currentFrame << " : '" << error << "' while reading '" << helper.getPathStringAtHash(hash) << '\'' << std::endl;
        }
    }
}

const std::vector<ImageHolder>& FileBufferHolder::getImages() const {
    return m_Images;
}

