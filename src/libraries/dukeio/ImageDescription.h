/*
 * ImageDescription.h
 *
 *  Created on: 5 mai 2010
 *      Author: Guillaume Chatelet
 */

#ifndef IMAGEDESCRIPTION_H_
#define IMAGEDESCRIPTION_H_

#include "Formats.h"
#include <cstddef>
#include <cstdarg>

enum ImageSpecFlag {
    IS_FLAG_UPSIDE_DOWN = 1 << 0, ///< the image should be flipped over the horizontal axis
    IS_LAST
///< last enum marker
};

typedef struct ImageDescription {
    // spatial attributes
    int width; ///< width of the pixel data window
    int height; ///< height of the pixel data window
    int depth; ///< depth of pixel data, >1 indicates a "volume"
    // pixel format
    TPixelFormat format;
    // data itself
    const char* pFileData; ///< a pointer to the start of the file, NULL if not available
    std::size_t fileDataSize; ///< the size of the file
    const char* pImageData; ///< a pointer to the start of uncompressed data, NULL if not available
    std::size_t imageDataSize; ///< the size of the buffer

    ImageDescription() :
        width(0), height(0), depth(0), format(PXF_UNDEFINED), pFileData(NULL), fileDataSize(0), pImageData(NULL), imageDataSize(0) {
    }

    bool blank() const {
        return width == 0 && height == 0 && depth == 0 && format == PXF_UNDEFINED;
    }
} ImageDescription;

#endif /* IMAGEDESCRIPTION_H_ */
