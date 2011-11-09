#ifndef FILEBUFFERHOLDER_H_
#define FILEBUFFERHOLDER_H_

#include "ImageHolder.h"

#include <boost/noncopyable.hpp>
#include <vector>

class SmartCache;
struct PlaylistHelper;

class FileBufferHolder : public boost::noncopyable {
private:
    typedef std::vector<ImageHolder> Images;
    Images m_Images;
    std::vector<std::size_t> m_Iterators;
    std::size_t m_LastFrame;

public:
    FileBufferHolder();

    void update(const std::size_t frame, const SmartCache& cache, const PlaylistHelper& helper);

    const Images& getImages() const;
};

#endif /* FILEBUFFERHOLDER_H_ */
