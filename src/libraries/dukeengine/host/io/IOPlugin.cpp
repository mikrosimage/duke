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
