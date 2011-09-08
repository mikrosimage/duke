/*
 * DpxDecoder.cpp
 *
 *  Created on: 5 mai 2010
 *      Author: Guillaume Chatelet
 */

#include "DpxDecoder.h"
#include <duke_io/openfx/api/ofxDukeIo.h>
#include <boost/bind.hpp>
#include <iostream>
#include <cstring>

namespace {
#define DPX_MAGIC           0x53445058
#define DPX_MAGIC_SWAP      0x58504453

typedef struct file_information {
    unsigned int magic_num; /* magic number 0x53445058 (SDPX) big endian or 0x58504453 (XPDS) little endian */
    unsigned int offset; /* offset to image data in bytes */
    char vers[8]; /* which header format version is being used (v1.0)*/
    unsigned int file_size; /* file size in bytes */
    unsigned int ditto_key; /* read time short cut - 0 = same, 1 = new */
    unsigned int gen_hdr_size; /* generic header length in bytes */
    unsigned int ind_hdr_size; /* industry header length in bytes */
    unsigned int user_data_size; /* user-defined data length in bytes */
    char file_name[100]; /* image file name */
    char create_time[24]; /* file creation date "yyyy:mm:dd:hh:mm:ss:LTZ" */
    char creator[100]; /* file creator's name */
    char project[200]; /* project name */
    char copyright[200]; /* right to use or copyright info */
    unsigned int key; /* encryption ( FFFFFFFF = unencrypted ) */
    char Reserved[104]; /* reserved field TBD (need to pad) */
} FileInformation;

typedef struct _image_information {
    unsigned short orientation; /* image orientation */
    unsigned short element_number; /* number of image elements */
    unsigned int pixels_per_line; /* or x value */
    unsigned int lines_per_image_ele; /* or y value, per element */
    struct _image_element {
        unsigned int data_sign; /* data sign (0 = unsigned, 1 = signed ) */
        /* "Core set images are unsigned" */
        unsigned int ref_low_data; /* reference low data code value */
        float ref_low_quantity; /* reference low quantity represented */
        unsigned int ref_high_data; /* reference high data code value */
        float ref_high_quantity; /* reference high quantity represented */
        unsigned char descriptor; /* descriptor for image element */
        unsigned char transfer; /* transfer characteristics for element */
        unsigned char colorimetric; /* colormetric specification for element */
        unsigned char bit_size; /* bit size for element */
        unsigned short packing; /* packing for element */
        unsigned short encoding; /* encoding for element */
        unsigned int data_offset; /* offset to data of element */
        unsigned int eol_padding; /* end of line padding used in element */
        unsigned int eo_image_padding; /* end of image padding used in element */
        char description[32]; /* description of element */
    } image_element[8]; /* NOTE THERE ARE EIGHT OF THESE */

    unsigned char reserved[52]; /* reserved for future use (padding) */
} Image_Information;

} // namespace

DpxDecoder::DpxDecoder() :
    m_PropertySuite(*this) {
    registerAction(kOfxActionLoad, boost::bind(&DpxDecoder::noOp, this));
    registerAction(kOfxActionUnload, boost::bind(&DpxDecoder::noOp, this));
    registerAction(kOfxActionDescribe, boost::bind(&DpxDecoder::describe, this, _1, _2, _3));
    registerAction(kOfxDukeIoActionReadHeader, boost::bind(&DpxDecoder::readHeader, this, _1, _2, _3));
    registerAction(kOfxDukeIoActionDecodeImage, boost::bind(&DpxDecoder::noOp, this));
}

DpxDecoder::~DpxDecoder() {
}

OfxStatus DpxDecoder::noOp() {
    return kOfxStatOK;
}

OfxStatus DpxDecoder::describe(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out) {
    openfx::plugin::PropertyHelper helper = m_PropertySuite.getHelper(out);
    helper.setString(kOfxDukeIoSupportedExtensions, "dpx");
    helper.setInt(kOfxDukeIoUncompressedFormat, 1);
    helper.setInt(kOfxDukeIoDelegateRead, 1);
    return kOfxStatOK;
}

OfxStatus DpxDecoder::readHeader(const void* handle, OfxPropertySetHandle in, OfxPropertySetHandle out) {
    try {
        int width = 0, height = 0, depth = 0, format = 0;
        void * pUncompressedData;
        openfx::plugin::PropertyHelper inArgHelper = m_PropertySuite.getHelper(in);
        std::string filename = inArgHelper.getString(kOfxDukeIoImageFilename);
        void * pData = inArgHelper.getPointer(kOfxDukeIoImageFileDataPtr);
        int dataSize = inArgHelper.getInt(kOfxDukeIoImageFileDataSize);

        if (dataSize == 0)
            return kOfxStatFailed;

        const FileInformation* pInformation = reinterpret_cast<const FileInformation*> (pData);
        char* pArithmeticPointer = reinterpret_cast<char*> (pData);
        const Image_Information* pImageInformation = reinterpret_cast<const Image_Information*> (pArithmeticPointer + sizeof(FileInformation));

        const unsigned int magic = pInformation->magic_num;

        if (magic != DPX_MAGIC_SWAP && magic != DPX_MAGIC) {
            std::cerr << "invalid magic : not a dpx file" << std::endl;
            return kOfxStatFailed;
        }

        if (magic == DPX_MAGIC_SWAP) {
            height = __builtin_bswap32(pImageInformation->lines_per_image_ele);
            width = __builtin_bswap32(pImageInformation->pixels_per_line);
            pUncompressedData = pArithmeticPointer + __builtin_bswap32(pInformation->offset);
            format = kOfxDukeIoImageFormatR10G10B10A2;
        } else {
            height = pImageInformation->lines_per_image_ele;
            width = pImageInformation->pixels_per_line;
            pUncompressedData = pArithmeticPointer + pInformation->offset;
            format = kOfxDukeIoImageFormatA2B10G10R10;
        }
        dataSize = height * width * sizeof(int32_t);

        openfx::plugin::PropertyHelper outArgHelper = m_PropertySuite.getHelper(out);
        outArgHelper.setInt(kOfxDukeIoImageFormat, format);
        outArgHelper.setInt(kOfxDukeIoImageWidth, width);
        outArgHelper.setInt(kOfxDukeIoImageHeight, height);
        outArgHelper.setInt(kOfxDukeIoImageDepth, depth);
        outArgHelper.setInt(kOfxDukeIoBufferSize, dataSize);
        outArgHelper.setPointer(kOfxDukeIoBufferPtr, pUncompressedData);
        return kOfxStatOK;
    } catch (std::exception& e) {
        std::cerr << "unhandled exception in Dpx plugin : " << e.what() << std::endl;
        return kOfxStatFailed;
    }
    return kOfxStatFailed;
}

//
// Main entry
//
const int PLUGIN_COUNT = 1;

// You should include PluginBootstrap.h only once as it defines global variables and functions
#include <openfx/support/plugin/PluginBootstrap.h>

OfxPluginInstance<0> plug(kOfxDukeIoApi, kOfxDukeIoApiVersion, "fr.mikrosimage.optimized_dpx", 1, 0, new DpxDecoder());
