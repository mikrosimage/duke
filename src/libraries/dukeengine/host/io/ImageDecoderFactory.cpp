#include "ImageDecoderFactory.h"
#include "ImageDecoderFactoryImpl.h"

ImageDecoderFactory::ImageDecoderFactory() :
    m_pImpl(new ImageDecoderFactoryImpl()) {
}

ImageDecoderFactory::~ImageDecoderFactory() {
}

FormatHandle ImageDecoderFactory::getImageDecoder(const char* extension, bool &delegateRead, bool &isFormatUncompressed) const {
    // TODO remove locking and use TLS
    boost::lock_guard<boost::mutex> lock(m_Mutex);
    return m_pImpl->getImageDecoder(extension, delegateRead, isFormatUncompressed);
}

bool ImageDecoderFactory::readImageHeader(const char* filename, FormatHandle decoder, ImageDescription& description) const {
    // TODO remove locking and use TLS
    boost::lock_guard<boost::mutex> lock(m_Mutex);
    return m_pImpl->readImageHeader(filename, decoder, description);
}

bool ImageDecoderFactory::decodeImage(FormatHandle decoder, const ImageDescription& description) const {
    // TODO remove locking and use TLS
    boost::lock_guard<boost::mutex> lock(m_Mutex);
    return m_pImpl->decodeImage(decoder, description);
}
