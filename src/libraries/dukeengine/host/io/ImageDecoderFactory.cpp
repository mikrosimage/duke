/*
 * ImageDecoderFactory.cpp
 *
 *  Created on: 7 mai 2010
 *      Author: Guillaume Chatelet
 */

#include "ImageDecoderFactory.h"
#include "ImageDecoderFactoryImpl.h"

ImageDecoderFactory::ImageDecoderFactory() :
    m_pImpl(new ImageDecoderFactoryImpl()) {
}

ImageDecoderFactory::~ImageDecoderFactory() {
}

FormatHandle ImageDecoderFactory::getImageDecoder(const char* extension, bool &delegateRead, bool &isFormatUncompressed) const {
    return m_pImpl->getImageDecoder(extension, delegateRead, isFormatUncompressed);
}

bool ImageDecoderFactory::readImageHeader(const char* filename, FormatHandle decoder, ImageDescription& description) const {
    return m_pImpl->readImageHeader(filename, decoder, description);
}

bool ImageDecoderFactory::decodeImage(FormatHandle decoder, const ImageDescription& description) const {
    return m_pImpl->decodeImage(decoder, description);
}
