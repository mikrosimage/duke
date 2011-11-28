#include "IOPlugin.h"
#include <dukeio/ofxDukeIo.h>
#include <dukehost/HostUtils.h>

#include <vector>
#include <string>

using namespace std;

static inline void split(const string& s, char c, vector<string>& v) {
    string::size_type i = 0;
    string::size_type j = s.find(c);
    while (j != string::npos) {
        v.push_back(s.substr(i, j - i));
        i = ++j;
        j = s.find(c, j);
        if (j == string::npos)
            v.push_back(s.substr(i, s.length()));
    }
    if (v.empty())
        v.push_back(s);
}

IOPlugin::IOPlugin(const OfxPlugin &plugin) :
    m_Plugin(plugin), m_ExtensionProperty(kOfxDukeIoSupportedExtensions, ""), m_UncompressedFormat(kOfxDukeIoUncompressedFormat, 0), m_DelegateRead(kOfxDukeIoDelegateRead, 0) {
    m_DescribeProperty << m_ExtensionProperty << m_UncompressedFormat << m_DelegateRead;
    ::openfx::host::perform(&m_Plugin, kOfxActionDescribe, &m_DescribeProperty, NULL, NULL);
    split(m_ExtensionProperty.value[0], ',', m_Extensions);
}

IOPlugin::~IOPlugin() {
}

const vector<string>& IOPlugin::extensions() const {
    return m_Extensions;
}

bool IOPlugin::uncompressedFormat() const {
    return m_UncompressedFormat.value[0] == 1;
}

bool IOPlugin::delegateRead() const {
    return m_DelegateRead.value[0] == 1;
}

IOPluginInstance::IOPluginInstance(const IOPlugin & plugin) :
    m_Plugin(plugin.m_Plugin) {
    ::openfx::host::perform(&m_Plugin, kOfxActionCreateInstance, &m_InstanceProperty, NULL, NULL);
}

IOPluginInstance::~IOPluginInstance() {
    ::openfx::host::perform(&m_Plugin, kOfxActionDestroyInstance, &m_InstanceProperty, NULL, NULL);
}

#include <dukeio/ImageDescription.h>
#include <dukehost/suite/property/OstreamHelpers.h>

bool IOPluginInstance::readImageHeader(const char *filename, ImageDescription & description) const {
    using namespace ::openfx::host;
    StringProperty filenameProp(kOfxDukeIoImageFilename, filename);
    PointerProperty fileDataProp(kOfxDukeIoImageFileDataPtr, const_cast<char*> (description.pFileData));
    IntProperty fileSizeProp(kOfxDukeIoImageFileDataSize, description.fileDataSize);
    ::openfx::host::PropertySet inArgs;
    inArgs << filenameProp << fileDataProp << fileSizeProp;

    IntProperty imageFormatProp(kOfxDukeIoImageFormat, kOfxDukeIoImageFormatUndefined);
    IntProperty imageWidthProp(kOfxDukeIoImageWidth, 0);
    IntProperty imageHeightProp(kOfxDukeIoImageHeight, 0);
    IntProperty imageDepthProp(kOfxDukeIoImageDepth, 0);
    PointerProperty imageDataProp(kOfxDukeIoBufferPtr, NULL);
    IntProperty imageSizeProp(kOfxDukeIoBufferSize, 0);
    ::openfx::host::PropertySet outArgs;
    outArgs << imageFormatProp << imageWidthProp << imageHeightProp << imageDepthProp;
    outArgs << imageDataProp << imageSizeProp;

    const OfxStatus status = ::openfx::host::perform(&m_Plugin, kOfxDukeIoActionReadHeader, NULL, &inArgs, &outArgs);
    // TPixelFormat are guaranteed to match exactly the DukeIO Format thanks to the enum definition
    // taking DukeIO API values as initialization.
    description.format = static_cast<TPixelFormat> (imageFormatProp.value[0]);
    description.width = imageWidthProp.value[0];
    description.height = imageHeightProp.value[0];
    description.depth = imageDepthProp.value[0];
    description.pImageData = static_cast<char const*> (const_cast<void const*> (imageDataProp.value[0]));
    description.imageDataSize = imageSizeProp.value[0];
    return status == kOfxStatOK;
}

bool IOPluginInstance::decodeImage(const ImageDescription & description) const {
    assert(description.pImageData);
    using namespace ::openfx::host;
    PointerProperty imageDataProp(kOfxDukeIoBufferPtr, const_cast<char*> (description.pImageData));
    IntProperty imageSizeProp(kOfxDukeIoBufferSize, description.imageDataSize);
    IntProperty imageWidthProp(kOfxDukeIoImageWidth, description.width);
    IntProperty imageHeightProp(kOfxDukeIoImageHeight, description.height);
    IntProperty imageDepthProp(kOfxDukeIoImageDepth, description.depth);
    PointerProperty fileDataProp(kOfxDukeIoImageFileDataPtr, const_cast<char*> (description.pFileData));
    IntProperty fileSizeProp(kOfxDukeIoImageFileDataSize, description.fileDataSize);

    ::openfx::host::PropertySet inArgs;
    inArgs << imageDataProp << imageSizeProp << imageWidthProp << imageHeightProp << imageDepthProp << fileDataProp << fileSizeProp;

    return ::openfx::host::perform(&m_Plugin, kOfxDukeIoActionDecodeImage, NULL, &inArgs, NULL) == kOfxStatOK;
}

