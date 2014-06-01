#pragma once

#include "duke/image/Channel.hpp"
#include "duke/attributes/Attributes.hpp"

struct ImageDescription {
  int32_t x = 0;        // origin (left corner) of pixel data
  int32_t y = 0;        // origin (upper corner) of pixel data
  uint32_t width = 0;   // width of the pixel data window
  uint32_t height = 0;  // height of the pixel data window

  int32_t full_x = 0;        // origin of the full (display) window
  int32_t full_y = 0;        // origin of the full (display) window
  uint32_t full_width = 0;   // width of the full (display) window
  uint32_t full_height = 0;  // height of the full (display) window

  uint32_t tile_width = 0;   // tile width (0 for a non-tiled image)
  uint32_t tile_height = 0;  // tile height (0 for a non-tiled image)

  Channels channels;           // channel's types and names
  int32_t opengl_format = -1;  // opengl format representing the channels, or -1 if not available

  attribute::Attributes extra_attributes;  // additional attributes
};
