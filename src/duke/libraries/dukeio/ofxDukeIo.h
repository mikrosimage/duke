#ifndef OFXDUKEIO_H_
#define OFXDUKEIO_H_

// String used to label DukeIO Plug-ins
#define kOfxDukeIoApi "DukeIO image input/output"

// Current version of this API
#define kOfxDukeIoApiVersion 1

/**
 * Constants
 */

// Image pixel formats
#define kOfxDukeIoImageFormatUndefined          ( (int)0 )
#define kOfxDukeIoImageFormatR8G8B8A8           ( (int)1 )
#define kOfxDukeIoImageFormatB8G8R8A8           ( (int)2 )
#define kOfxDukeIoImageFormatR10G10B10A2        ( (int)3 )
#define kOfxDukeIoImageFormatA2B10G10R10        ( (int)4 )
#define kOfxDukeIoImageFormatR16G16B16A16F      ( (int)5 )
#define kOfxDukeIoImageFormatB16G16R16A16F      ( (int)6 )
#define kOfxDukeIoImageFormatR32G32B32A32F      ( (int)7 )
#define kOfxDukeIoImageFormatB32G32R32A32F      ( (int)8 )
#define kOfxDukeIoImageFormatR8G8B8	     	     ( (int)9 )
#define kOfxDukeIoImageFormatB8G8R8       	 	 ( (int)10 )
#define kOfxDukeIoImageFormatR16G16B16F	  	     ( (int)11 )
#define kOfxDukeIoImageFormatB16G16R16F    	 	 ( (int)12 )
#define kOfxDukeIoImageFormatR32G32B32F  	     ( (int)13 )
#define kOfxDukeIoImageFormatB32G32R32F  	 	 ( (int)14 )
#define kOfxDukeIoImageFormatR32F       	 	 ( (int)15 )

/**
 * Actions
 */
#define kOfxDukeIoActionReadHeader              "OfxDukeIoActionReadHeader"
#define kOfxDukeIoActionDecodeImage             "OfxDukeIoActionDecodeImage"

/**
 * Plugin properties
 * action               : kOfxActionDescribe
 * OfxPropertySetHandle : outArgs
 */
#define kOfxDukeIoSupportedExtensions           "OfxDukeIoSupportedExtensions" ///< String property, comma separated
#define kOfxDukeIoUncompressedFormat            "OfxDukeIoUncompressedFormat" ///< Int property, 0=false, 1=true
#define kOfxDukeIoDelegateRead                  "OfxDukeIoDelegateRead" ///< Int property, 0=false, 1=true

/**
 * Image properties
 * action               : kOfxDukeIoActionReadHeader
 */
// OfxPropertySetHandle : inArgs
#define kOfxDukeIoImageFilename                 "OfxDukeIoImageFilename"
#define kOfxDukeIoImageFileDataPtr              "OfxDukeIoImageFileDataPtr"
#define kOfxDukeIoImageFileDataSize             "OfxDukeIoImageFileDataSize"
// OfxPropertySetHandle : outArgs
#define kOfxDukeIoImageFormat                   "OfxDukeIoImageFormat"
#define kOfxDukeIoImageWidth                    "OfxDukeIoImageWidth"
#define kOfxDukeIoImageHeight                   "OfxDukeIoImageHeight"
#define kOfxDukeIoImageDepth                    "OfxDukeIoImageDepth"

#define kOfxDukeIoBufferPtr	                    "OfxDukeIoBufferPtr"
#define kOfxDukeIoBufferSize                    "OfxDukeIoBufferSize"

#endif /* OFXDUKEIO_H_ */
