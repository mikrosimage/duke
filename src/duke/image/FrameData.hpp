#pragma once

#include <duke/base/Slice.hpp>
#include <duke/image/ImageDescription.hpp>

#include <memory>

struct Allocator;

namespace duke {

struct FrameData {
  // Sets the description for this frame, allocate a buffer to hold the frame bytes and returns it.
  // Subsequent calls to getData will return an immutable view of the allocated buffer.
  Slice<char> setDescriptionAndAllocate(const ImageDescription& description, const Allocator& allocator);

  // Sets the description for this frame and stores an immutable view of some memory region that correspond
  // Subsequent calls to getData will return this same data.
  void setDescriptionAndVolatileData(const ImageDescription& description, ConstMemorySlice data);

  // If data needs to outlive the reader call this function to make sure its lifetime is be tied to this FrameData
  // object.
  void persistDataIfNeeded(const Allocator& allocator);

  // Get the memory slice corresponding to the frame's buffer.
  // Returns empty slice if unset.
  ConstMemorySlice getData() const { return m_FrameData; }

  const ImageDescription& getDescription() const { return m_Description; }

  void updateOpenGlFormat();

 private:
  ImageDescription m_Description;
  std::shared_ptr<char> m_pData;
  ConstMemorySlice m_FrameData;
};

} /* namespace duke */
