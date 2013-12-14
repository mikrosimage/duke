#pragma once

#include <duke/base/NonCopyable.hpp>
#include <duke/engine/streams/IIOOperation.hpp>
#include <duke/attributes/Attributes.hpp>

#include <string>
#include <mutex>

namespace duke {

struct MediaFrameReference;

class IMediaStream: public noncopyable {
public:
	virtual ~IMediaStream() {};

    // This function can be called from different threads.
    virtual InputFrameOperationResult process(const MediaFrameReference& mfr) const = 0;

	Attributes getAttributes() const {
	    std::lock_guard<std::mutex> guard(m_Mutex);
	    return m_Attributes;
	}
	void setAttributes(Attributes&& attributes) {
	    std::lock_guard<std::mutex> guard(m_Mutex);
	    m_Attributes = std::move(attributes);
	}
	void setAttributes(const Attributes& attributes) {
	    std::lock_guard<std::mutex> guard(m_Mutex);
	    m_Attributes = attributes;
	}

private:
	mutable std::mutex m_Mutex;
    Attributes m_Attributes; // be sure to lock all access.
};

} /* namespace duke */
