/*
 * Renderer.cpp
 *
 *  Created on: 14 avr. 2010
 *      Author: Guillaume Chatelet
 */

#include "Renderer.h"
#include <openfx/support/host/HostUtils.h>
#include <openfx/support/plugin/OstreamHelper.h>
#include <renderer/common/ofxRenderer.h>
#include <boost/thread.hpp>
#include <sstream>
#include <iostream>
#include <cassert>
#include <stdexcept>

using namespace std;

Renderer::Renderer(OfxHost host, const char* filename) :
    m_Host(host), m_Plugin(filename), m_pOfxPlugin(NULL), m_bIsInit(false) {
    m_Plugin.load(m_Host);
    const vector<OfxPlugin*> plugins(m_Plugin.getPlugins());
    if (plugins.size() > 1)
        cout << "More than one plug-in found in " << filename << ". Only the first one will be taken into account." << endl;
    m_pOfxPlugin = plugins[0];
    if (string(kOfxRendererPluginApi) != m_pOfxPlugin->pluginApi) {
        ostringstream msg("Incorrect pluginApi, should be ");
        msg << kOfxRendererPluginApi;
        msg << " but was ";
        msg << m_pOfxPlugin->pluginApi;
        throw runtime_error(msg.str());
    }
    cout << "[Renderer] " << *m_pOfxPlugin << endl;
}

Renderer::~Renderer() {
    waitForRenderLoopToStop();
}

void Renderer::initRender(const ::protocol::duke::Renderer& Renderer) {
    ::openfx::host::perform(m_pOfxPlugin, kOfxActionRendererInit, &Renderer, NULL, NULL);
    m_bIsInit = true;
}

void Renderer::waitForRenderLoopToStop() {
    if (m_bIsInit) {
        ::openfx::host::perform(m_pOfxPlugin, kOfxActionWaitForMainLoopEnd, NULL, NULL, NULL);
        m_bIsInit = false;
    }
}

