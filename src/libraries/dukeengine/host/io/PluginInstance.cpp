/*
 * PluginInstance.cpp
 *
 *  Created on: 30 sept. 2010
 *      Author: Guillaume Chatelet
 */

#include "PluginInstance.h"
#include <openfx/support/host/HostUtils.h>
#include <duke_io/openfx/api/ofxDukeIo.h>

#include <vector>
#include <string>

PluginInstance::PluginInstance(const OfxPlugin &plugin) :
    m_ExtensionProperty(kOfxDukeIoSupportedExtensions, ""), m_UncompressedFormat(kOfxDukeIoUncompressedFormat, 0), m_DelegateRead(kOfxDukeIoDelegateRead, 0), m_Plugin(plugin) {
    m_DescribeProperty << m_ExtensionProperty << m_UncompressedFormat << m_DelegateRead;
    ::openfx::host::perform(&m_Plugin, kOfxActionDescribe, NULL, NULL, &m_DescribeProperty);
}

PluginInstance::~PluginInstance() {
}

void split(const std::string& s, char c, std::vector<std::string>& v) {
    std::string::size_type i = 0;
    std::string::size_type j = s.find(c);
    while (j != std::string::npos) {
        v.push_back(s.substr(i, j - i));
        i = ++j;
        j = s.find(c, j);
        if (j == std::string::npos)
            v.push_back(s.substr(i, s.length()));
    }
    if (v.empty())
        v.push_back(s);
}

std::vector<std::string> PluginInstance::extensions() const {
    std::vector<std::string> extensions;
    split(m_ExtensionProperty.value[0], ',', extensions);
    return extensions;
}

bool PluginInstance::uncompressedFormat() const {
    return m_UncompressedFormat.value[0] == 1;
}

bool PluginInstance::delegateRead() const {
    return m_DelegateRead.value[0] == 1;
}
