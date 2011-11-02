#include "ImageDecoderFactoryImpl.h"

#include <dukehost/HostUtils.h>
#include <dukeplugin/OstreamHelper.h>

#include <dukeio/ofxDukeIo.h>
#include <iostream>

using namespace std;

static bool _acceptFile(const char* filename, const bool isDirectory) {
    return isDirectory || string(filename).find("plugin_io_") != string::npos;
}

static bool _acceptPlug(const OfxPlugin* plug) {
    if (string(kOfxDukeIoApi) != plug->pluginApi) {
        cerr << "DukeIO API not handled by plugin " << plug->pluginIdentifier << endl;
        return false;
    }
    if (kOfxDukeIoApiVersion != plug->apiVersion) {
        cerr << "DukeIO API version not handled by plugin " << plug->pluginIdentifier << endl;
        return false;
    }
    return true;
}

static void displayPlugin(const string &extension, const IOPlugin& plugin) {
    cout << "[DukeIO] " << plugin.m_Plugin << " \'" << extension << "\'";
    if (plugin.uncompressedFormat())
        cout << "\t[OPTIMIZED]";
    if (plugin.delegateRead())
        cout << "\t[DELEGATE_READ]";
    cout << endl;
}

ImageDecoderFactoryImpl::ImageDecoderFactoryImpl() :
    m_PluginManager(*this, ".", &_acceptFile, &_acceptPlug) {
    // browsing the plugins and building the extension provider

    using ::openfx::host::PluginManager;
    const PluginManager::PluginVector plugins(m_PluginManager.getPlugins());
    for (PluginManager::PluginVector::const_iterator itr = plugins.begin(); itr != plugins.end(); ++itr) {
        boost::shared_ptr<IOPlugin> pPlugin(new IOPlugin(**itr));
        typedef vector<string> Vector;
        const Vector extensions = pPlugin->extensions();
        for (Vector::const_iterator itr = extensions.begin(); itr != extensions.end(); ++itr)
            m_Map[*itr] = pPlugin;
    }
    // display help
    for (ExtensionToDecoderMap::const_iterator itr = m_Map.begin(); itr != m_Map.end(); ++itr)
        displayPlugin(itr->first, *itr->second);
}

ImageDecoderFactoryImpl::~ImageDecoderFactoryImpl() {
}

FormatHandle ImageDecoderFactoryImpl::getImageDecoder(const char* extension, bool &delegateRead, bool &isFormatUncompressed) const {
    const char* validExt = extension;
    if (validExt[0] == '.')
        ++validExt;
    const ExtensionToDecoderMap::const_iterator mapEntry(m_Map.find(validExt));
    if (mapEntry == m_Map.end())
        return NULL;
    const SharedIOPlugin &pIOPlugin(mapEntry->second);
    isFormatUncompressed = pIOPlugin->uncompressedFormat();
    delegateRead = pIOPlugin->delegateRead();
    return pIOPlugin.get();
}

#include <dukehost/suite/property/PropertySet.h>
#include <dukehost/suite/property/Property.h>
#include <dukehost/suite/property/OstreamHelpers.h>

OfxStatus perform(FormatHandle decoder, const char* action, const void* handle, OfxPropertySetHandle inArgs, OfxPropertySetHandle outArgs) {
    assert(decoder);
    const IOPlugin* pIOPlugin = reinterpret_cast<const IOPlugin*> (decoder);
    return ::openfx::host::perform(&pIOPlugin->m_Plugin, action, NULL, inArgs, outArgs);
}

bool ImageDecoderFactoryImpl::readImageHeader(FormatHandle decoder, const char* filename, ImageDescription& description) const {
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

    OfxStatus status = perform(decoder, kOfxDukeIoActionReadHeader, NULL, &inArgs, &outArgs);
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

bool ImageDecoderFactoryImpl::decodeImage(FormatHandle decoder, const ImageDescription& description) const {
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
    return perform(decoder, kOfxDukeIoActionDecodeImage, NULL, &inArgs, NULL) == kOfxStatOK;
}

