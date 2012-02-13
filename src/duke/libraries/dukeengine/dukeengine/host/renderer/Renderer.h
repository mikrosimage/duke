#ifndef RENDERER_H_
#define RENDERER_H_

#include <openfx/ofxCorePlugin.h>
#include <dukehost/PluginBinary.h>

#include <player.pb.h>

class Renderer {
private:
    OfxHost m_Host;
    ::openfx::host::PluginBinary m_Plugin;
    OfxPlugin *m_pOfxPlugin;
    bool m_bIsInit;

public:
    Renderer(OfxHost host, const char* filename);
    ~Renderer(); // not virtual

    void initRender(const ::duke::protocol::Renderer& Renderer);
    void waitForRenderLoopToStop();

    inline bool isInit() const {
        return m_bIsInit;
    }
};

#endif /* RENDERER_H_ */
