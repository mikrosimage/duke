#pragma

#include <duke/image/ChannelType.hpp>
#include <duke/attributes/Attributes.hpp>

struct ImageDescription {
  int32_t x, y;     // origin (upper left corner) of pixel data
  uint32_t width;   // width of the pixel data window
  uint32_t height;  // height of the pixel data window

  int32_t full_x;        // origin of the full (display) window
  int32_t full_y;        // origin of the full (display) window
  uint32_t full_width;   // width of the full (display) window
  uint32_t full_height;  // height of the full (display) window

  uint32_t tile_width;   // tile width (0 for a non-tiled image)
  uint32_t tile_height;  // tile height (0 for a non-tiled image)

  uint8_t nchannels;                             // number of image channels, e.g., 4 for RGBA
  ImageChannelType format;                       // data format of the channels
  std::vector<ImageChannelType> channelformats;  // Optional per-channel formats
  std::vector<std::string> channelnames;         // Names for each channel e.g., {"R","G","B","A"}
  int8_t alpha_channel;                          // Index of alpha channel, or -1 if not known
  int8_t z_channel;                              // Index of depth channel, or -1 if not known

  attribute::Attributes extra_attributes;  // Additional attributes
};
