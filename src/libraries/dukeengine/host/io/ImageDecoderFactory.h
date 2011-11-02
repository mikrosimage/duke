#ifndef IMAGEDECODERFACTORY_H_
#define IMAGEDECODERFACTORY_H_

#include <dukeio/ImageDescription.h>

typedef void* FormatHandle;

struct ImageDecoderFactory {
    virtual FormatHandle getImageDecoder(const char* extension, bool &delegateRead, bool &isFormatUncompressed) const = 0;
    virtual bool readImageHeader(const char* filename, FormatHandle decoder, ImageDescription& description) const = 0;
    virtual bool decodeImage(FormatHandle decoder, const ImageDescription& description) const = 0;
};

#endif /* IMAGEDECODERFACTORY_H_ */
