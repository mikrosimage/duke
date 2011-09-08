/*
 * ImageHolder.cpp
 *
 *  Created on: 5 mai 2010
 *      Author: Guillaume Chatelet
 */

#include "ImageHolder.h"

ImageHolder::ImageHolder() {
}

ImageHolder::~ImageHolder() {
}

void ImageHolder::setImageData(const ImageDescription &description, const MemoryBlockPtr pMemory) {
    m_ImageDescription = description;
    m_pImageData = pMemory;
    if (m_ImageDescription.pImageData == NULL)
        m_ImageDescription.pImageData = m_pImageData->getPtr<char> ();
}

size_t ImageHolder::getImageDataSize() const {
    if (m_pImageData == NULL)
        return 0;
    return m_ImageDescription.imageDataSize;
}

