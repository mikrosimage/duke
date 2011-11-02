#include "OIIODecoder.h"
#include <dukeio/ofxDukeIo.h>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <imageio.h>
#include <iostream>

using namespace OpenImageIO::v0;

OIIODecoder::OIIODecoder() :
    m_pImageInput(NULL),
    m_PropertySuite(*this) {
    registerAction(kOfxActionLoad, boost::bind(&OIIODecoder::noOp, this));
    registerAction(kOfxActionUnload, boost::bind(&OIIODecoder::noOp, this));
    registerAction(kOfxActionDescribe, boost::bind(&OIIODecoder::describe, this, _1, _2, _3));
    registerAction(kOfxDukeIoActionReadHeader, boost::bind(&OIIODecoder::readHeader, this, _1, _2, _3));
    registerAction(kOfxDukeIoActionDecodeImage, boost::bind(&OIIODecoder::decodeImage, this, _1, _2, _3));
}

OIIODecoder::~OIIODecoder() {
}

OfxStatus OIIODecoder::noOp() {
    return kOfxStatOK;
}

OfxStatus OIIODecoder::describe(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out) {
    openfx::plugin::PropertyHelper helper = m_PropertySuite.getHelper(out);
    helper.setString(kOfxDukeIoSupportedExtensions, "bmp,dds,hdr,ico,jp2,j2k,exr,png,pbm,pgm,ppm,pnm,sgi,rgb,pic,tga,tpic,tif,tiff");
    helper.setInt(kOfxDukeIoUncompressedFormat, 0);
    helper.setInt(kOfxDukeIoDelegateRead, 0);
    return kOfxStatOK;
}

OfxStatus OIIODecoder::readHeader(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out) {
    try {
        int width = 0, height = 0, depth = 0, format = 0;
        openfx::plugin::PropertyHelper inArgHelper = m_PropertySuite.getHelper(in);
        std::string filename = inArgHelper.getString(kOfxDukeIoImageFilename);

        ImageSpec spec;
        m_pImageInput = ImageInput::create(filename);
        if(!m_pImageInput)
          return kOfxStatFailed;

        if(!m_pImageInput->open(filename, spec)) {
            delete m_pImageInput;
            return kOfxStatFailed;
        }

        width = spec.width;
        height = spec.height;
        //TODO depth

        if(spec.nchannels == 3)
            format = kOfxDukeIoImageFormatR16G16B16F;
        else if(spec.nchannels == 4)
            format = kOfxDukeIoImageFormatR16G16B16A16F;
        else {
            std::cerr << "OIIO plugin: image format not handled yet (nchannels must be 3 or 4) \n";
            m_pImageInput->close();
            delete m_pImageInput;
            return kOfxStatFailed;
        }

//        // ------- DUMP
//        std::cerr << "width: " << spec.width << std::endl;
//        std::cerr << "full_width: " << spec.full_width << std::endl;
//        std::cerr << "height: " << spec.height << std::endl;
//        std::cerr << "full_height: " << spec.full_height << std::endl;
//        std::cerr << "x: " << spec.x << std::endl;
//        std::cerr << "y: " << spec.y << std::endl;
//        std::cerr << "z: " << spec.z << std::endl;
//        std::cerr << "full_x: " << spec.full_x << std::endl;
//        std::cerr << "full_y: " << spec.full_y << std::endl;
//        std::cerr << "full_z: " << spec.full_z << std::endl;
//        std::cerr << "tile_width: " << spec.tile_width << std::endl;
//        std::cerr << "tile_height: " << spec.tile_height << std::endl;
//        std::cerr << "tile_depth: " << spec.tile_depth << std::endl;
//        std::cerr << "nchannels: " << spec.nchannels << std::endl;
//        std::cerr << "alpha_channel: " << spec.alpha_channel << std::endl;
//        std::cerr << "z_channel: " << spec.z_channel << std::endl;
//        std::cerr << "channelnames: " ;
//        BOOST_FOREACH(std::string s, spec.channelnames){std::cerr << s << ", ";}
//        std::cerr << std::endl ;

        int dataSize = height * width * spec.nchannels * sizeof(float)/2 ;
        openfx::plugin::PropertyHelper outArgHelper = m_PropertySuite.getHelper(out);
        outArgHelper.setInt(kOfxDukeIoImageFormat, format);
        outArgHelper.setInt(kOfxDukeIoImageWidth, width);
        outArgHelper.setInt(kOfxDukeIoImageHeight, height);
        outArgHelper.setInt(kOfxDukeIoImageDepth, depth);
        outArgHelper.setInt(kOfxDukeIoBufferSize, dataSize);
        return kOfxStatOK;

    } catch (std::exception& e) {
        std::cerr << "Unhandled exception in OIIO plugin: " << e.what() << std::endl;
    }
    m_pImageInput->close();
    delete m_pImageInput;
    return kOfxStatFailed;

}

OfxStatus OIIODecoder::decodeImage(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out) {
    try {

        if (!m_pImageInput)
            return kOfxStatFailed;
        openfx::plugin::PropertyHelper inArgHelper = m_PropertySuite.getHelper(in);
        void * pData = inArgHelper.getPointer(kOfxDukeIoBufferPtr);
        m_pImageInput->read_image(TypeDesc::HALF, pData);
        m_pImageInput->close();
        delete m_pImageInput;

    } catch (std::exception& e) {
        std::cerr << "unhandled exception in OIIO plugin: " << e.what() << std::endl;
        delete m_pImageInput;
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

OfxPluginInstance<0> plug(kOfxDukeIoApi, kOfxDukeIoApiVersion, "fr.mikrosimage.openImageIO", 1, 0, new OIIODecoder());
