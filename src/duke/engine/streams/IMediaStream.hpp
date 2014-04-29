#pragma once

#include <duke/base/NonCopyable.hpp>
#include <duke/engine/streams/IIOOperation.hpp>
#include <duke/attributes/Attributes.hpp>

#include <string>
#include <mutex>

namespace duke {

class IMediaStream : public noncopyable {
public:
    virtual ~IMediaStream() {}

    // This function can be called from different threads.
    virtual InputFrameOperationResult process(const size_t frame) const = 0;

    // If true this stream is seekable and random access.
    virtual bool isForward() const = 0;

    // Contains the state of the stream as well as all the metadata
    const attribute::Attributes& getState() const {
        return m_State;
    }
protected:
    attribute::Attributes m_State;
};

} /* namespace duke */
