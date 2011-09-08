#ifndef OIIODECODER_H_
#define OIIODECODER_H_

#include <dukeio/ImageDescription.h>
#include <dukeplugin/IBoostPlugin.h>
#include <dukeplugin/suite/property/PropertySuiteImpl.h>

namespace OpenImageIO{
namespace v0{
	class ImageInput;
} // namespace v0
} // namespace OpenImageIO

class OIIODecoder : public IBoostPlugin
{
	OpenImageIO::v0::ImageInput * m_pImageInput;
    openfx::plugin::PropertySuiteImpl m_PropertySuite;

public:
    OIIODecoder();
	virtual ~OIIODecoder();

	OfxStatus noOp();
    OfxStatus describe(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out);
    OfxStatus readHeader(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out);
    OfxStatus decodeImage(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out);
};

#endif /* OIIODECODER_H_ */
