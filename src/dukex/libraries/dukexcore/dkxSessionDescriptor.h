#ifndef DKXSESSIONDESCRIPTOR_H
#define DKXSESSIONDESCRIPTOR_H

#include "player.pb.h"

class SessionDescriptor {

public:
    SessionDescriptor() {
    }
    SessionDescriptor(const SessionDescriptor& d) :
        mRenderer(d.renderer()), mScene(d.scene()), mDisplayMode(d.displayMode()) {
    }
    const SessionDescriptor& operator=(const SessionDescriptor& d) {
        mRenderer = d.renderer();
        mScene = d.scene();
        mDisplayMode = d.displayMode();
        return *this;
    }

public:
    inline ::duke::protocol::Renderer & renderer() {
        return mRenderer;
    }
    inline const ::duke::protocol::Renderer & renderer() const {
        return mRenderer;
    }
    inline ::duke::protocol::Scene & scene() {
        return mScene;
    }
    inline const ::duke::protocol::Scene & scene() const {
        return mScene;
    }
    inline ::duke::protocol::StaticParameter & displayMode() {
        return mDisplayMode;
    }
    inline const ::duke::protocol::StaticParameter & displayMode() const {
        return mDisplayMode;
    }

private:
    ::duke::protocol::Renderer mRenderer;
    ::duke::protocol::Scene mScene;
    ::duke::protocol::StaticParameter mDisplayMode;
};

#endif // DKXSESSIONDESCRIPTOR_H
