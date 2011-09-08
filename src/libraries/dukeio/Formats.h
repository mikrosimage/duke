#ifndef FORMATS_H_
#define FORMATS_H_

#include <duke_io/openfx/api/ofxDukeIo.h>

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

#endif /* FORMATS_H_ */

