#ifndef OIIODECODER_H_
#define OIIODECODER_H_

#include <dukeio/ImageDescription.h>
#include <dukeplugin/IBoostPlugin.h>
#include <dukeplugin/suite/property/PropertySuiteImpl.h>

#include <boost/thread/tss.hpp>


namespace OpenImageIO{
namespace v0{
	class ImageInput;
} // namespace v0
} // namespace OpenImageIO

class OIIODecoder : public IBoostPlugin
{
public:
    typedef OpenImageIO::v0::ImageInput ImageInput;
    typedef boost::thread_specific_ptr<ImageInput> ImageInputPtr;
private:
    ImageInputPtr m_pImageInput;
    openfx::plugin::PropertySuiteImpl m_PropertySuite;

    OfxStatus safeClose();
    void safeSet(ImageInput *pImage);
public:
    OIIODecoder();
	virtual ~OIIODecoder();

	OfxStatus noOp();
    OfxStatus describe(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out);
    OfxStatus readHeader(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out);
    OfxStatus decodeImage(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out);
};

#endif /* OIIODECODER_H_ */
