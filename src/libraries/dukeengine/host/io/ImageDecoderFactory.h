#ifndef IMAGEDECODERFACTORY_H_
#define IMAGEDECODERFACTORY_H_

#include <dukeio/ImageDescription.h>
#include <memory>
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>

typedef void* FormatHandle;
class ImageDecoderFactoryImpl;

class ImageDecoderFactory {
private:
    std::auto_ptr<ImageDecoderFactoryImpl> m_pImpl;
    mutable boost::mutex m_Mutex;

public:
    ImageDecoderFactory();
    ~ImageDecoderFactory();

    FormatHandle getImageDecoder(const char* extension, bool &delegateRead, bool &isFormatUncompressed) const;
    bool readImageHeader(const char* filename, FormatHandle decoder, ImageDescription& description) const;
    bool decodeImage(FormatHandle decoder, const ImageDescription& description) const;
};

#endif /* IMAGEDECODERFACTORY_H_ */
