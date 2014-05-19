#pragma once

#include <duke/attributes/Attributes.hpp>
#include <duke/base/NonCopyable.hpp>
#include <duke/imageio/IIOOperation.hpp>

namespace duke {

class IMediaStream : public noncopyable {
 public:
  virtual ~IMediaStream() {}

  // Returns a description of the current stream.
  virtual const IImageReader& getImageReader() const = 0;

  // This function can be called from different threads.
  virtual ReadFrameResult process(const size_t frame) const = 0;

  // True if this stream is only a forward stream
  virtual bool isForwardOnly() const = 0;

  // Contains the state of the stream like current frame for movie stream
  virtual const attribute::Attributes& getState() const = 0;
};

} /* namespace duke */
