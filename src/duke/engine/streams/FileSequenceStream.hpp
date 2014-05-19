#pragma once

#include "IMediaStream.hpp"

#include <duke/attributes/Attributes.hpp>
#include <duke/imageio/DukeIO.hpp>

#include <vector>
#include <string>

namespace sequence {
struct Item;
}

namespace duke {

class FileSequenceStream final : public duke::IMediaStream {
 public:
  FileSequenceStream(const sequence::Item& item);
  ~FileSequenceStream() override {}

  const IImageReader& getImageReader() const override;

  // This function can be called from different threads.
  ReadFrameResult process(const size_t frame) const override;

  // File sequences are random access streams
  bool isForwardOnly() const override { return false; }

  const attribute::Attributes& getState() const override { return m_State; }

 private:
  const size_t m_FrameStart;
  const size_t m_Padding;
  const std::vector<IIODescriptor*> m_Descriptors;
  std::string m_Prefix;
  std::string m_Suffix;
  ReadFrameResult m_OpenResult;
  attribute::Attributes m_State;
};

}  // namespace duke
