#include "ImageDecoderFactoryImpl.h"

#include <dukehost/HostUtils.h>
#include <dukeplugin/OstreamHelper.h>

#include <dukeio/ofxDukeIo.h>
#include <iostream>

#include <cassert>

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
    for (PluginManager::PluginVector::const_iterator itr = plugins.begin(), end = plugins.end(); itr != end; ++itr) {
        assert(*itr!=NULL); // itr must not be NULL
        boost::shared_ptr<IOPlugin> pPlugin(new IOPlugin(**itr));
        for (vector<string>::const_iterator itr = pPlugin->extensions().begin(), e = pPlugin->extensions().end(); itr != e; ++itr)
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

ImageDecoderFactoryImpl::SharedIOPluginInstance ImageDecoderFactoryImpl::getTLSPluginInstance(FormatHandle decoder) const {
    assert(decoder);
    const IOPlugin* const pPlugin = reinterpret_cast<const IOPlugin*> (decoder);
    if (m_pPluginToInstance.get() == NULL)
        m_pPluginToInstance.reset(new PluginToInstance());
    const PluginToInstance::const_iterator itr = m_pPluginToInstance->find(pPlugin);
    if (itr == m_pPluginToInstance->end()) {
        SharedIOPluginInstance pInstance(new IOPluginInstance(*pPlugin));
        m_pPluginToInstance->insert(std::make_pair(pPlugin, pInstance));
        return pInstance;
    }
    return itr->second;
}

bool ImageDecoderFactoryImpl::readImageHeader(FormatHandle decoder, const char* filename, ImageDescription& description) const {
    assert(decoder);
    return getTLSPluginInstance(decoder)->readImageHeader(filename, description);
}

bool ImageDecoderFactoryImpl::decodeImage(FormatHandle decoder, const ImageDescription& description) const {
    assert(decoder);
    return getTLSPluginInstance(decoder)->decodeImage(description);
}

