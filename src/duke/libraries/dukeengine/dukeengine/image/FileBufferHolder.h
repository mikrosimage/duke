#ifndef FILEBUFFERHOLDER_H_
#define FILEBUFFERHOLDER_H_

#include "ImageHolder.h"

#include <dukeapi/sequence/PlaylistHelper.h>

#include <boost/noncopyable.hpp>
#include <vector>

struct SmartCache;

class FileBufferHolder : public boost::noncopyable {
private:
    std::vector<ImageHolder> m_Images;
    duke::protocol::MediaFrames m_MediaFrames;

public:
    void update(const std::size_t frame, const SmartCache& cache, const duke::protocol::PlaylistHelper& helper);
    const std::vector<ImageHolder>& getImages() const;
};

#endif /* FILEBUFFERHOLDER_H_ */
