/*
 * ImageDecoderFactory.h
 *
 *  Created on: 7 mai 2010
 *      Author: Guillaume Chatelet
 */

#ifndef IMAGEDECODERFACTORY_H_
#define IMAGEDECODERFACTORY_H_

#include "ImageDescription.h"
#include <memory>

typedef void* FormatHandle;
class ImageDecoderFactoryImpl;

class ImageDecoderFactory {
private:
    std::auto_ptr<ImageDecoderFactoryImpl> m_pImpl;
public:
    ImageDecoderFactory();
    ~ImageDecoderFactory();

    FormatHandle getImageDecoder(const char* extension, bool &delegateRead, bool &isFormatUncompressed) const;
    bool readImageHeader(const char* filename, FormatHandle decoder, ImageDescription& description) const;
    bool decodeImage(FormatHandle decoder, const ImageDescription& description) const;
};

#endif /* IMAGEDECODERFACTORY_H_ */
