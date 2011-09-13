#include "FileBufferHolder.h"
#include <dukeengine/image/ImageReader.h>
#include <dukeengine/sequence/PlaylistHelper.h>
#include <dukeio/ImageDescription.h>
#include <boost/foreach.hpp>

using namespace std;

FileBufferHolder::FileBufferHolder(const ImageReader& imageReader) :
    m_ImageReader(imageReader), m_Cache(0/*200 * 1024 * 1024*/), m_LastFrame(-1) {
}

static bool get(const ImageReader& imageReader, const PlaylistHelper &helper, const size_t itrIndex, ImageHolder& value) {
    const uint64_t hash = helper.getHashAtIterator(itrIndex);
    if (hash == 0)
        return false;

    const boost::filesystem::path filename = helper.getPathAtHash(hash);
    if (filename.empty())
        return true;

    return imageReader.load(filename, value);
}

void FileBufferHolder::update(const uint32_t currentFrame, const PlaylistHelper &helper) {
    if (m_LastFrame == currentFrame)
        return;
    m_Images.clear();

    helper.getIteratorsAtFrame(currentFrame, m_Iterators);

    BOOST_FOREACH( const size_t &itr, m_Iterators )
                {
                    boost::filesystem::path p = helper.getPathAtIterator(itr);

                    // pushing an empty description
                    m_Images.push_back(ImageHolder());

                    const bool inCache = m_Cache.get(p.string(), m_Images.back());
                    if (!inCache) {
                        // filling the description
                        if (!get(m_ImageReader, helper, itr, m_Images.back())) {
                            std::cerr << "unable to get frame " << itr << std::endl;
                            continue;
                        }
                        m_Cache.put(p.string(), m_Images.back());
                    }
                }
    m_LastFrame = currentFrame;
}

const FileBufferHolder::Images& FileBufferHolder::getImages() const {
    return m_Images;
}

