#pragma once

#include <string>
#include <vector>

struct Channel {
  enum class Semantic {
    UNKNOWN,
    RED,
    GREEN,
    BLUE,
    ALPHA,
    DEPTH,
  };

  Channel(Semantic numeric_type, uint8_t bits, const std::string& name = "")
      : semantic(numeric_type), bits(bits), name(name) {}
  Channel() : Channel(Semantic::UNKNOWN, 0) {}

  Semantic semantic;
  uint8_t bits;
  std::string name;
};

struct Channels : public std::vector<Channel> {
  enum class FormatType {
    UNKNOWN,
    UNSIGNED_NORMALIZED,
    SIGNED_NORMALIZED,
    UNSIGNED_INTEGRAL,
    SIGNED_INTEGRAL,
    FLOATING_POINT,
  };

  std::string asString() const;

  FormatType type = FormatType::UNKNOWN;
};
