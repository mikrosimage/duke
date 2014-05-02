#pragma once

#include <cstddef>
#include <tuple>

struct FrameDescription {
  size_t width, height;
  size_t glFormat;  // corresponds to OpenGL internal image format
  size_t dataSize;
  bool swapEndianness;
  bool swapRedAndBlue;

  FrameDescription() : width(0), height(0), glFormat(0), dataSize(0), swapEndianness(false), swapRedAndBlue(false) {}

  bool operator<(const FrameDescription &other) const { return asTuple() < other.asTuple(); }

 private:
  inline const std::tuple<size_t, size_t, size_t, size_t> asTuple() const {
    return std::make_tuple(width, height, glFormat, dataSize);
  }
};
