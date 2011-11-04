#ifndef IOPLUGIN_H_
#define IOPLUGIN_H_

#include <openfx/ofxCorePlugin.h>
#include <dukehost/suite/property/PropertySet.h>
#include <dukehost/suite/property/Property.h>

struct ImageDescription;
class IOPlugin;

class IOPluginInstance {
public:
    IOPluginInstance(const IOPlugin &);
    virtual ~IOPluginInstance();
    virtual bool readImageHeader(const char* filename, ImageDescription& description) const;
    virtual bool decodeImage(const ImageDescription& description) const;
private:
    const OfxPlugin &m_Plugin;
    ::openfx::host::PropertySet m_InstanceProperty;
};

class IOPlugin {
public:
    const OfxPlugin &m_Plugin;
    IOPlugin(const OfxPlugin &plugin);
    virtual ~IOPlugin();

    const std::vector<std::string>& extensions() const;
    bool delegateRead() const;
    bool uncompressedFormat() const;
private:
    ::openfx::host::PropertySet m_DescribeProperty;
    ::openfx::host::StringProperty m_ExtensionProperty;
    ::openfx::host::IntProperty m_UncompressedFormat;
    ::openfx::host::IntProperty m_DelegateRead;
    std::vector<std::string> m_Extensions;
};

#endif /* IOPLUGIN_H_ */
