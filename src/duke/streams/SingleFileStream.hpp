#pragma once

#include "IMediaStream.hpp"

#include "duke/attributes/Attributes.hpp"
#include "duke/io/IO.hpp"

#include <mutex>

namespace sequence {
struct Item;
}

namespace duke {

class SingleFileStream final : public duke::IMediaStream {
 public:
  SingleFileStream(const sequence::Item& item);
  ~SingleFileStream() override {}

  const ReadFrameResult& getResult() const override;

  // This function can be called from different threads.
  ReadFrameResult process(const size_t frame) const override;

  // True if this stream is a movie
  bool isForwardOnly() const override;

  const attribute::Attributes& getState() const override { return m_State; }

 private:
  mutable std::mutex m_Mutex;
  ReadFrameResult m_OpenResult;
  attribute::Attributes m_State;
};

}  // namespace duke
