#include "FileBufferHolder.h"

#include <dukeengine/image/SmartCache.h>
#include <dukeengine/sequence/PlaylistHelper.h>
#include <dukeio/ImageDescription.h>

#include <boost/foreach.hpp>

#include <iostream>

using namespace std;

FileBufferHolder::FileBufferHolder() :
        m_LastFrame(-1) {
}

void FileBufferHolder::update(const std::size_t currentFrame, const SmartCache& cache, const PlaylistHelper& helper) {
    if (m_LastFrame == currentFrame)
        return;
    m_Images.clear();

    helper.getIteratorsAtFrame(currentFrame, m_Iterators);
    BOOST_FOREACH(const uint32_t itr, m_Iterators) {
        m_Images.push_back(ImageHolder()); // pushing an empty description
        if (!cache.get(helper.getHashAtIterator(itr), m_Images.back()))
            std::cerr << "unable to get frame " << currentFrame << std::endl;
    }
    m_LastFrame = currentFrame;
}

const std::vector<ImageHolder>& FileBufferHolder::getImages() const {
    return m_Images;
}

