#ifndef DPXDECODER_H_
#define DPXDECODER_H_

#include <dukeio/ImageDescription.h>
#include <dukeplugin/IBoostPlugin.h>
#include <dukeplugin/suite/property/PropertySuiteImpl.h>

class DpxDecoder : public IBoostPlugin {

    openfx::plugin::PropertySuiteImpl m_PropertySuite;

public:
    DpxDecoder();
    virtual ~DpxDecoder();

    OfxStatus noOp();
    OfxStatus createInstance(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out);
    OfxStatus destroyInstance(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out);
    OfxStatus describe(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out);
    OfxStatus readHeader(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out);
};

#endif /* DPXDECODER_H_ */
