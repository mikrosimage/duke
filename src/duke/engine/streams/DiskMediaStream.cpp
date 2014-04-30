#include "DiskMediaStream.hpp"

#include <duke/attributes/AttributeKeys.hpp>
#include <duke/base/Check.hpp>
#include <duke/engine/streams/SingleFileStream.hpp>
#include <duke/engine/streams/FileSequenceStream.hpp>
#include <sequence/Item.hpp>

#include <algorithm>
#include <memory>

namespace duke {

DiskMediaStream::DiskMediaStream(const attribute::Attributes& options, const sequence::Item& item) {
  switch (item.getType()) {
    case sequence::Item::SINGLE:
      m_pDelegate.reset(new SingleFileStream(options, item));
      break;
    case sequence::Item::PACKED:
      m_pDelegate.reset(new FileSequenceStream(options, item));
      break;
    default:
      CHECK(!"Invalid state");
      break;
  }
  CHECK(m_pDelegate);
}

InputFrameOperationResult DiskMediaStream::process(const size_t frame) const {
  return CHECK_NOTNULL(m_pDelegate)->process(frame);
}

bool DiskMediaStream::isForwardOnly() const { return CHECK_NOTNULL(m_pDelegate)->isForwardOnly(); }

const attribute::Attributes& DiskMediaStream::getState() const { return CHECK_NOTNULL(m_pDelegate)->getState(); }

} /* namespace duke */
