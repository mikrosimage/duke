#include "FileBufferHolder.h"

#include <dukeengine/image/SmartCache.h>
#include <dukeengine/sequence/PlaylistHelper.h>
#include <dukeio/ImageDescription.h>

#include <boost/foreach.hpp>

using namespace std;

FileBufferHolder::FileBufferHolder() :
    m_LastFrame(-1) {
}

//static bool get(const ImageReader& imageReader, const PlaylistHelper &helper, const size_t itrIndex, ImageHolder& value) {
//    const uint64_t hash = helper.getHashAtIterator(itrIndex);
//    if (hash == 0)
//        return false;
//
//    const boost::filesystem::path filename = helper.getPathAtHash(hash);
//    if (filename.empty())
//        return true;
//
//    return imageReader.load(filename, value);
//}

//void FileBufferHolder::update(const uint32_t currentFrame, const PlaylistHelper &helper) {
//    if (m_LastFrame == currentFrame)
//        return;
//    m_Images.clear();
//
//    helper.getIteratorsAtFrame(currentFrame, m_Iterators);
//    BOOST_FOREACH( const size_t &itr, m_Iterators )
//                {
//                    boost::filesystem::path p = helper.getPathAtIterator(itr);
//
//                    // pushing an empty description
//                    m_Images.push_back(ImageHolder());
//
//                    // filling the description
//                    if (!get(m_ImageReader, helper, itr, m_Images.back())) {
//                        std::cerr << "unable to get frame " << itr << std::endl;
//                        continue;
//                    }
//                }
//    m_LastFrame = currentFrame;
//}

void FileBufferHolder::update(const std::size_t currentFrame, const SmartCache& cache, const PlaylistHelper& helper) {
    if (m_LastFrame == currentFrame)
        return;
    m_Images.clear();

    helper.getIteratorsAtFrame(currentFrame, m_Iterators);
    BOOST_FOREACH( const uint32_t &itr, m_Iterators )
                {
                    m_Images.push_back(ImageHolder()); // pushing an empty description
                    if (!cache.get(helper.getHashAtIterator(itr), m_Images.back()))
                        std::cerr << "unable to get frame " << currentFrame << " (image data not available)" << std::endl;
                }
    m_LastFrame = currentFrame;
}

const FileBufferHolder::Images& FileBufferHolder::getImages() const {
    return m_Images;
}

