#pragma once

#include <string>
#include <vector>

struct ImageChannelType {
  enum {
    SIGNED,
    UNSIGNED,
    FLOAT,
  } numeric_type;
  enum {
    BYTE_8,
    BYTE_16,
    BYTE_32,
    BYTE_64,
  } numeric_sizeof;
};
