#pragma once

#include <duke/base/NonCopyable.hpp>
#include <duke/engine/streams/IIOOperation.hpp>
#include <duke/attributes/Attributes.hpp>

#include <string>
#include <mutex>

namespace duke {

struct MediaFrameReference;

class IMediaStream : public noncopyable {
public:
    virtual ~IMediaStream() {}

    // This function can be called from different threads.
    virtual InputFrameOperationResult process(const size_t frame) const = 0;

    // If true the this stream is random access and
    // can easily seek in the stream.
    virtual bool isFileSequence() const = 0;

    const Attributes& getState() const {
        return m_StreamAttributes;
    }
protected:
    Attributes m_StreamAttributes;
};

} /* namespace duke */
