#include "duke/image/ImageUtils.hpp"

#include "duke/base/Check.hpp"

size_t getChannelsByteSize(const Channels& channels) {
  size_t bits = 0;
  for (const auto& channel : channels) bits += channel.bits;
  CHECK(bits % 8 == 0);
  return bits / 8;
}

size_t getImageSize(const ImageDescription& description) {
  return description.width * description.height * getChannelsByteSize(description.channels);
}
