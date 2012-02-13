#include "FileBufferHolder.h"

#include <sequence/DisplayUtils.h>
#include <dukeengine/image/SmartCache.h>
#include <dukeio/ImageDescription.h>

#include <boost/foreach.hpp>

#include <iostream>

using namespace std;
using namespace duke::protocol;

ostream& operator<<(ostream& stream, const MediaFrame &mf){
    stream << mf.item;
    return stream;
}

void FileBufferHolder::update(const std::size_t currentFrame, const SmartCache& cache, const PlaylistHelper& helper) {
    m_Images.clear();

    helper.mediaFramesAt(currentFrame, m_MediaFrames);
    BOOST_FOREACH(const MediaFrame& mf, m_MediaFrames)
    {
        m_Images.push_back(ImageHolder()); // pushing an empty description
        if (!cache.get(mf, m_Images.back())) {
            const string &error = m_Images.back().error;
            std::cerr << "At frame " << currentFrame << " : '" << error << "' while reading '" << mf << '\'' << std::endl;
        }
    }
}

const std::vector<ImageHolder>& FileBufferHolder::getImages() const {
    return m_Images;
}

