#include "duke/image/FrameData.hpp"

#include "duke/base/Check.hpp"
#include "duke/gl/GlUtils.hpp"
#include "duke/image/ImageUtils.hpp"
#include "duke/memory/Allocator.hpp"

namespace duke {

Slice<char> FrameData::setDescriptionAndAllocate(const ImageDescription& description, const Allocator& allocator) {
  CHECK(!m_pData) << "must be called once";
  m_Description = description;
  const auto size = getImageSize(m_Description);
  m_pData = make_shared_memory<char>(size, allocator);
  m_FrameData = {m_pData.get(), m_pData.get() + size};
  return {m_pData.get(), m_pData.get() + size};
}

void FrameData::setDescriptionAndVolatileData(const ImageDescription& description, ConstMemorySlice data) {
  m_Description = description;
  m_FrameData = data;
}

void FrameData::persistDataIfNeeded(const Allocator& allocator) {
  const bool isDataAllocated = m_pData && m_pData.get() == m_FrameData.begin();
  if (!isDataAllocated) {
    const auto size = m_FrameData.size();
    m_pData = make_shared_memory<char>(size, allocator);
    memcpy(m_pData.get(), m_FrameData.begin(), size);
    m_FrameData = {m_pData.get(), m_pData.get() + size};
  }
}

void FrameData::updateOpenGlFormat() {
  m_Description.opengl_format = getOpenGlFormat(m_Description.channels);
  CHECK(m_Description.opengl_format != -1) << "Unable to set OpenGl Format from channel definition";
}
}  // namespace duke
