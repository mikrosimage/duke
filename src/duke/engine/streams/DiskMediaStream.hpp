#pragma once

#include "IMediaStream.hpp"

namespace sequence {
struct Item;
}

namespace duke {

class DiskMediaStream final : public duke::IMediaStream {
 public:
  DiskMediaStream(const attribute::Attributes& readerOptions, const sequence::Item& item);

  InputFrameOperationResult process(const size_t frame) const override;

  bool isForwardOnly() const override;

  const attribute::Attributes& getState() const override;

  size_t getFrameCount() const;

 private:
  std::unique_ptr<IMediaStream> m_pDelegate;
};

} /* namespace duke */
