/*
 * TurboJpegDecoder.cpp
 *
 *  Created on: 18 oct. 2010
 *      Author: Nicolas Rondaud
 */

#include "TurboJpegDecoder.h"
#include <dukeio/ofxDukeIo.h>
#include <boost/bind.hpp>
#include <iostream>
#include <stdio.h>


TurboJpegDecoder::TurboJpegDecoder() :
	m_PropertySuite(*this), m_jpeghandle(tjInitDecompress()) {
    registerAction(kOfxActionLoad, boost::bind(&TurboJpegDecoder::noOp, this));
    registerAction(kOfxActionUnload, boost::bind(&TurboJpegDecoder::noOp, this));
    registerAction(kOfxActionDescribe, boost::bind(&TurboJpegDecoder::describe, this, _1, _2, _3));
    registerAction(kOfxDukeIoActionReadHeader, boost::bind(&TurboJpegDecoder::readHeader, this, _1, _2, _3));
    registerAction(kOfxDukeIoActionDecodeImage, boost::bind(&TurboJpegDecoder::decodeImage, this, _1, _2, _3));
}

TurboJpegDecoder::~TurboJpegDecoder() {
    tjDestroy(m_jpeghandle);
}

OfxStatus TurboJpegDecoder::noOp() {
    return kOfxStatOK;
}

OfxStatus TurboJpegDecoder::describe(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out) {
    openfx::plugin::PropertyHelper helper = m_PropertySuite.getHelper(out);
    helper.setString(kOfxDukeIoSupportedExtensions, "jpeg,jpg");
    helper.setInt(kOfxDukeIoUncompressedFormat, 0);
    helper.setInt(kOfxDukeIoDelegateRead, 1);
    return kOfxStatOK;
}

OfxStatus TurboJpegDecoder::readHeader(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out) {
    try {
        openfx::plugin::PropertyHelper inArgHelper = m_PropertySuite.getHelper(in);
        int width = 0;
        int height = 0;
        int depth = 1;
        int jpegsubsamp;
        int format = kOfxDukeIoImageFormatR8G8B8;
        tjDecompressHeader2(m_jpeghandle, (unsigned char*)inArgHelper.getPointer(kOfxDukeIoImageFileDataPtr), inArgHelper.getInt(kOfxDukeIoImageFileDataSize), &width, &height, &jpegsubsamp);
        openfx::plugin::PropertyHelper outArgHelper = m_PropertySuite.getHelper(out);
        outArgHelper.setInt(kOfxDukeIoImageFormat, format);
        outArgHelper.setInt(kOfxDukeIoImageWidth, width);
        outArgHelper.setInt(kOfxDukeIoImageHeight, height);
        outArgHelper.setInt(kOfxDukeIoImageDepth, depth);
        outArgHelper.setInt(kOfxDukeIoBufferSize, width*height*3*8);
        return kOfxStatOK;
    } catch (std::exception& e) {
        std::cerr << "unhandled exception in Jpeg plugin : " << e.what() << std::endl;
        return kOfxStatFailed;
    }
    return kOfxStatFailed;
}

OfxStatus TurboJpegDecoder::decodeImage(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out) {
    try {
        openfx::plugin::PropertyHelper inArgHelper = m_PropertySuite.getHelper(in);
        void * pFileData = inArgHelper.getPointer(kOfxDukeIoImageFileDataPtr);
        void * pData = inArgHelper.getPointer(kOfxDukeIoBufferPtr);
        int fileSize = inArgHelper.getInt(kOfxDukeIoImageFileDataSize);
        int width = inArgHelper.getInt(kOfxDukeIoImageWidth);
        int height = inArgHelper.getInt(kOfxDukeIoImageHeight);
        tjDecompress(m_jpeghandle, (unsigned char*)pFileData, fileSize, (unsigned char*)pData, width, 0, height, 3, TJ_FASTUPSAMPLE);
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
