/*
 * PluginInstance.h
 *
 *  Created on: 30 sept. 2010
 *      Author: Guillaume Chatelet
 */

#ifndef PLUGININSTANCE_H_
#define PLUGININSTANCE_H_

#include <openfx/ofxCorePlugin.h>
#include <dukehost/suite/property/PropertySet.h>
#include <dukehost/suite/property/Property.h>

class PluginInstance {
    ::openfx::host::PropertySet m_DescribeProperty;
    ::openfx::host::StringProperty m_ExtensionProperty;
    ::openfx::host::IntProperty m_UncompressedFormat;
    ::openfx::host::IntProperty m_DelegateRead;
public:
    const OfxPlugin &m_Plugin;
    PluginInstance(const OfxPlugin &plugin);
    virtual ~PluginInstance();

    std::vector<std::string> extensions() const;
    bool delegateRead() const;
    bool uncompressedFormat() const;
};

#endif /* PLUGININSTANCE_H_ */
