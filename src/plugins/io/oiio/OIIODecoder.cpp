#include "OIIODecoder.h"
#include <dukeio/ofxDukeIo.h>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <imageio.h>
#include <iostream>

using namespace std;
using namespace OpenImageIO::v0_11;

OIIODecoder::OIIODecoder() :
    m_pImageInput(NULL), m_PropertySuite(*this) {
    registerAction(kOfxActionLoad, boost::bind(&OIIODecoder::noOp, this));
    registerAction(kOfxActionUnload, boost::bind(&OIIODecoder::noOp, this));
    registerAction(kOfxActionDescribe, boost::bind(&OIIODecoder::describe, this, _1, _2, _3));
    registerAction(kOfxActionCreateInstance, boost::bind(&OIIODecoder::noOp, this));
    registerAction(kOfxActionDestroyInstance, boost::bind(&OIIODecoder::noOp, this));
    registerAction(kOfxDukeIoActionReadHeader, boost::bind(&OIIODecoder::readHeader, this, _1, _2, _3));
    registerAction(kOfxDukeIoActionDecodeImage, boost::bind(&OIIODecoder::decodeImage, this, _1, _2, _3));
}

OIIODecoder::~OIIODecoder() {
}

OfxStatus OIIODecoder::safeClose() {
    if (m_pImageInput.get()) {
        m_pImageInput->close();
        m_pImageInput.release();
    }
    return kOfxStatFailed;
}

void OIIODecoder::safeSet(ImageInput *pImage) {
    safeClose();
    m_pImageInput.reset(pImage);
}
OfxStatus OIIODecoder::noOp() {
    return kOfxStatOK;
}

OfxStatus OIIODecoder::describe(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out) {
    openfx::plugin::PropertyHelper helper = m_PropertySuite.getHelper(OfxPropertySetHandle(handle));
    helper.setString(kOfxDukeIoSupportedExtensions, "bmp,dds,hdr,ico,jp2,j2k,exr,png,pbm,pgm,ppm,pnm,sgi,rgb,pic,tga,tpic,tif,tiff");
    helper.setInt(kOfxDukeIoUncompressedFormat, 0);
    helper.setInt(kOfxDukeIoDelegateRead, 0);
    return kOfxStatOK;
}

OfxStatus OIIODecoder::readHeader(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out) {
    try {
        int width = 0, height = 0, depth = 0, format = 0;
        openfx::plugin::PropertyHelper inArgHelper = m_PropertySuite.getHelper(in);
        string filename = inArgHelper.getString(kOfxDukeIoImageFilename);

        safeSet(ImageInput::create(filename));

        if (m_pImageInput.get() == NULL)
            return kOfxStatFailed;

        ImageSpec spec;
        if (!m_pImageInput->open(filename, spec))
            return safeClose();

        width = spec.width;
        height = spec.height;
        //TODO depth

        if (spec.nchannels == 3)
            format = kOfxDukeIoImageFormatR16G16B16F;
        else if (spec.nchannels == 4)
            format = kOfxDukeIoImageFormatR16G16B16A16F;
        else {
            cerr << "OIIO plugin: image format not handled yet (nchannels must be 3 or 4) \n";
            return safeClose();
        }

        //        // ------- DUMP
        //        cerr << "width: " << spec.width << endl;
        //        cerr << "full_width: " << spec.full_width << endl;
        //        cerr << "height: " << spec.height << endl;
        //        cerr << "full_height: " << spec.full_height << endl;
        //        cerr << "x: " << spec.x << endl;
        //        cerr << "y: " << spec.y << endl;
        //        cerr << "z: " << spec.z << endl;
        //        cerr << "full_x: " << spec.full_x << endl;
        //        cerr << "full_y: " << spec.full_y << endl;
        //        cerr << "full_z: " << spec.full_z << endl;
        //        cerr << "tile_width: " << spec.tile_width << endl;
        //        cerr << "tile_height: " << spec.tile_height << endl;
        //        cerr << "tile_depth: " << spec.tile_depth << endl;
        //        cerr << "nchannels: " << spec.nchannels << endl;
        //        cerr << "alpha_channel: " << spec.alpha_channel << endl;
        //        cerr << "z_channel: " << spec.z_channel << endl;
        //        cerr << "channelnames: " ;
        //        BOOST_FOREACH(string s, spec.channelnames){cerr << s << ", ";}
        //        cerr << endl ;

        int dataSize = height * width * spec.nchannels * sizeof(float) / 2;
        openfx::plugin::PropertyHelper outArgHelper = m_PropertySuite.getHelper(out);
        outArgHelper.setInt(kOfxDukeIoImageFormat, format);
        outArgHelper.setInt(kOfxDukeIoImageWidth, width);
        outArgHelper.setInt(kOfxDukeIoImageHeight, height);
        outArgHelper.setInt(kOfxDukeIoImageDepth, depth);
        outArgHelper.setInt(kOfxDukeIoBufferSize, dataSize);
        return kOfxStatOK;

    } catch (exception& e) {
        cerr << "Unhandled exception in OIIO plugin: " << e.what() << endl;
    }
    return safeClose();
}

OfxStatus OIIODecoder::decodeImage(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out) {
    try {
        if (m_pImageInput.get() == NULL)
            return kOfxStatFailed;
        openfx::plugin::PropertyHelper inArgHelper = m_PropertySuite.getHelper(in);
        void * pData = inArgHelper.getPointer(kOfxDukeIoBufferPtr);
        m_pImageInput->read_image(TypeDesc::HALF, pData);
        safeClose();
    } catch (exception& e) {
        cerr << "unhandled exception in OIIO plugin: " << e.what() << endl;
        return safeClose();
    }
    return kOfxStatOK;
}

//
// Main entry
//
const int PLUGIN_COUNT = 1;

// You should include PluginBootstrap.h only once as it defines global variables and functions
#include <dukeplugin/PluginBootstrap.h>

OfxPluginInstance<0> plug(kOfxDukeIoApi, kOfxDukeIoApiVersion, "fr.mikrosimage.openImageIO", 1, 0, new OIIODecoder());
