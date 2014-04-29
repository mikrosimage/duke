#pragma once

#include "IMediaStream.hpp"
#include <sequence/Item.hpp>
#include <string>

namespace duke {

class IMediaStreamDelegate {
public:
    virtual ~IMediaStreamDelegate() {}

    virtual InputFrameOperationResult process(const size_t frame) const = 0;

    size_t frameCount = 0;
};

class DiskMediaStream : public duke::IMediaStream {
public:
    DiskMediaStream(const attribute::Attributes& readerOptions, const sequence::Item& item);

    virtual InputFrameOperationResult process(const size_t frame) const override {
        return m_pDelegate->process(frame);
    }

    virtual bool isForward() const override {
        return m_IsForward;
    }

    size_t getFrameCount() const {
        CHECK(m_pDelegate);
        return m_pDelegate->frameCount;
    }
private:
    std::unique_ptr<IMediaStreamDelegate> m_pDelegate;
    bool m_IsForward;
};

} /* namespace duke */
