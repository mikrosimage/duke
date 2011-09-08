#ifndef IMAGEREADER_H_
#define IMAGEREADER_H_

#include "ImageHolder.h"

#include <boost/noncopyable.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/date_time.hpp>

class ImageDecoderFactory;

class ImageReader : public boost::noncopyable {
    const ImageDecoderFactory& m_ImageFactory;
    mutable boost::posix_time::time_duration m_TimeWhole;
    mutable boost::posix_time::time_duration m_TimeGetDecoder;
    mutable boost::posix_time::time_duration m_TimeReadFile;
    mutable boost::posix_time::time_duration m_TimeReadHeader;
    mutable boost::posix_time::time_duration m_TimeDecode;
public:
    ImageReader(const ImageDecoderFactory&);
    virtual ~ImageReader();

    bool load(const boost::filesystem::path &path, ImageHolder &holder) const;
    void displayLastLoadStats() const;
};

#endif /* IMAGEREADER_H_ */
