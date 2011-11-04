#include "TurboJpegDecoder.h"
#include <dukeio/ofxDukeIo.h>
#include <boost/bind.hpp>
#include <iostream>
#include <stdio.h>

TurboJpegDecoder::TurboJpegDecoder() :
    m_PropertySuite(*this) {
    registerAction(kOfxActionLoad, boost::bind(&TurboJpegDecoder::noOp, this));
    registerAction(kOfxActionUnload, boost::bind(&TurboJpegDecoder::noOp, this));
    registerAction(kOfxActionDescribe, boost::bind(&TurboJpegDecoder::describe, this, _1, _2, _3));
    registerAction(kOfxActionCreateInstance, boost::bind(&TurboJpegDecoder::createInstance, this, _1, _2, _3));
    registerAction(kOfxActionDestroyInstance, boost::bind(&TurboJpegDecoder::destroyInstance, this, _1, _2, _3));
    registerAction(kOfxDukeIoActionReadHeader, boost::bind(&TurboJpegDecoder::readHeader, this, _1, _2, _3));
    registerAction(kOfxDukeIoActionDecodeImage, boost::bind(&TurboJpegDecoder::decodeImage, this, _1, _2, _3));
}

TurboJpegDecoder::~TurboJpegDecoder() {
}

OfxStatus TurboJpegDecoder::noOp() {
    return kOfxStatOK;
}

OfxStatus TurboJpegDecoder::describe(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out) {
    openfx::plugin::PropertyHelper helper = m_PropertySuite.getHelper(OfxPropertySetHandle(handle));
    helper.setString(kOfxDukeIoSupportedExtensions, "jpeg,jpg");
    helper.setInt(kOfxDukeIoUncompressedFormat, 0);
    helper.setInt(kOfxDukeIoDelegateRead, 1);
    return kOfxStatOK;
}

OfxStatus TurboJpegDecoder::createInstance(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out) {
    std::cout << "Creating instance" << std::endl;
    return kOfxStatOK;
}

OfxStatus TurboJpegDecoder::destroyInstance(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out) {
    std::cout << "Destroying instance" << std::endl;
    return kOfxStatOK;
}

OfxStatus TurboJpegDecoder::readHeader(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out) {
    try {
        const tjhandle jpeghandle = tjInitDecompress();
        openfx::plugin::PropertyHelper inArgHelper = m_PropertySuite.getHelper(in);
        int width = 0;
        int height = 0;
        int depth = 1;
        int jpegsubsamp;
        tjDecompressHeader2(jpeghandle, (unsigned char*) inArgHelper.getPointer(kOfxDukeIoImageFileDataPtr), inArgHelper.getInt(kOfxDukeIoImageFileDataSize), &width, &height,
                            &jpegsubsamp);
        //        int format = kOfxDukeIoImageFormatR8G8B8;
        int format = kOfxDukeIoImageFormatR8G8B8A8;

        openfx::plugin::PropertyHelper outArgHelper = m_PropertySuite.getHelper(out);
        outArgHelper.setInt(kOfxDukeIoImageFormat, format);
        outArgHelper.setInt(kOfxDukeIoImageWidth, width);
        outArgHelper.setInt(kOfxDukeIoImageHeight, height);
        outArgHelper.setInt(kOfxDukeIoImageDepth, depth);
        outArgHelper.setInt(kOfxDukeIoBufferSize, width * height * 4);
        tjDestroy(jpeghandle);
        return kOfxStatOK;
    } catch (std::exception& e) {
        std::cerr << "unhandled exception in Jpeg plugin : " << e.what() << std::endl;
        return kOfxStatFailed;
    }
    return kOfxStatFailed;
}

OfxStatus TurboJpegDecoder::decodeImage(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out) {
    try {
        const tjhandle jpeghandle = tjInitDecompress();
        openfx::plugin::PropertyHelper inArgHelper = m_PropertySuite.getHelper(in);
        void * pFileData = inArgHelper.getPointer(kOfxDukeIoImageFileDataPtr);
        void * pData = inArgHelper.getPointer(kOfxDukeIoBufferPtr);
        int fileSize = inArgHelper.getInt(kOfxDukeIoImageFileDataSize);
        int width = inArgHelper.getInt(kOfxDukeIoImageWidth);
        int height = inArgHelper.getInt(kOfxDukeIoImageHeight);
        tjDecompress(jpeghandle, (unsigned char*) pFileData, fileSize, (unsigned char*) pData, width, 0, height, 4, TJ_FASTUPSAMPLE);
        tjDestroy(jpeghandle);
    } catch (std::exception& e) {
        std::cerr << "unhandled exception in Jpeg plugin: " << e.what() << std::endl;
        return kOfxStatFailed;
    }
    return kOfxStatOK;
}

//
// Main entry
//
const int PLUGIN_COUNT = 1;

// You should include PluginBootstrap.h only once as it defines global variables and functions
#include <dukeplugin/PluginBootstrap.h>

OfxPluginInstance<0> plug(kOfxDukeIoApi, kOfxDukeIoApiVersion, "fr.mikrosimage.turbo_jpeg", 1, 0, new TurboJpegDecoder());
