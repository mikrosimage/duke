/*
 * DpxDecoder.h
 *
 *  Created on: 5 mai 2010
 *      Author: Guillaume Chatelet
 */

#ifndef DPXDECODER_H_
#define DPXDECODER_H_

#include <duke_io/ImageDescription.h>
#include <openfx/support/plugin/IBoostPlugin.h>
#include <openfx/support/plugin/suite/property/PropertySuiteImpl.h>

class DpxDecoder : public IBoostPlugin {

    openfx::plugin::PropertySuiteImpl m_PropertySuite;

public:
    DpxDecoder();
    virtual ~DpxDecoder();

    OfxStatus noOp();
    OfxStatus describe(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out);
    OfxStatus readHeader(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out);
};

#endif /* DPXDECODER_H_ */
