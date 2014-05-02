#pragma once

#include "IMediaStream.hpp"

#include <duke/attributes/Attributes.hpp>
#include <duke/imageio/DukeIO.hpp>

#include <vector>
#include <string>
#include <mutex>

namespace sequence {
struct Item;
}

namespace duke {

class SingleFileStream final : public duke::IMediaStream {
 public:
  SingleFileStream(const attribute::Attributes& options, const sequence::Item& item);
  ~SingleFileStream() override {}

  // This function can be called from different threads.
  ReadFrameResult process(const size_t frame) const override;

  // True if this stream is a movie
  bool isForwardOnly() const override;

  const attribute::Attributes& getState() const override { return m_State; }

 private:
  const std::string m_Filename;
  const std::vector<IIODescriptor*> m_Descriptors;
  mutable std::mutex m_Mutex;
  std::unique_ptr<IImageReader> m_pImageReader;
  attribute::Attributes m_State;
};

}  // namespace duke
