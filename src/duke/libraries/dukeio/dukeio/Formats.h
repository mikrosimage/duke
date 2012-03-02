#ifndef FORMATS_H_
#define FORMATS_H_

#include "ofxDukeIo.h"

/////////////////////////////////////////////////////////////
/// Pixel Format
/////////////////////////////////////////////////////////////
enum TPixelFormat
{
	PXF_UNDEFINED=kOfxDukeIoImageFormatUndefined,
	PXF_R8G8B8A8=kOfxDukeIoImageFormatR8G8B8A8,
	PXF_B8G8R8A8=kOfxDukeIoImageFormatB8G8R8A8,
	PXF_R10G10B10A2=kOfxDukeIoImageFormatR10G10B10A2,
	PXF_A2B10G10R10=kOfxDukeIoImageFormatA2B10G10R10,
	PXF_R16G16B16A16F=kOfxDukeIoImageFormatR16G16B16A16F,
	PXF_B16G16R16A16F=kOfxDukeIoImageFormatB16G16R16A16F,
	PXF_R32G32B32A32F=kOfxDukeIoImageFormatR32G32B32A32F,
	PXF_B32G32R32A32F=kOfxDukeIoImageFormatB32G32R32A32F,
	PXF_R8G8B8=kOfxDukeIoImageFormatR8G8B8,
	PXF_B8G8R8=kOfxDukeIoImageFormatB8G8R8,
	PXF_R16G16B16F=kOfxDukeIoImageFormatR16G16B16F,
	PXF_B16G16R16F=kOfxDukeIoImageFormatB16G16R16F,
	PXF_R32G32B32F=kOfxDukeIoImageFormatR32G32B32F,
	PXF_B32G32R32F=kOfxDukeIoImageFormatB32G32R32F,
	PXF_R32F=kOfxDukeIoImageFormatR32F
};

static inline const char* toString(TPixelFormat format) {
    switch (format) {
        case PXF_R8G8B8A8:
            return "R8G8B8A8";
        case PXF_B8G8R8A8:
            return "B8G8R8A8";
        case PXF_R10G10B10A2:
            return "R10G10B10A2";
        case PXF_A2B10G10R10:
            return "A2B10G10R10";
        case PXF_R16G16B16A16F:
            return "R16G16B16A16F";
        case PXF_B16G16R16A16F:
            return "B16G16R16A16F";
        case PXF_R32G32B32A32F:
            return "R32G32B32A32F";
        case PXF_B32G32R32A32F:
            return "B32G32R32A32F";
        case PXF_R8G8B8:
            return "R8G8B8";
        case PXF_B8G8R8:
            return "B8G8R8";
        case PXF_R16G16B16F:
            return "R16G16B16F";
        case PXF_B16G16R16F:
            return "B16G16R16F";
        case PXF_R32G32B32F:
            return "R32G32B32F";
        case PXF_B32G32R32F:
            return "B32G32R32F";
        case PXF_R32F:
            return "R32F";
        case PXF_UNDEFINED:
        default:
            return "undefined";
    }
}

#endif /* FORMATS_H_ */

