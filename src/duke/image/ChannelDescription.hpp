#pragma once

#include <string>
#include <vector>

struct ChannelType {
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

struct ChannelDescription : ChannelType {
  std::string name;
};

typedef std::vector<ChannelDescription> ChannelDescriptions;
