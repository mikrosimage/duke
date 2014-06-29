#include <duke/image/Channel.hpp>

namespace {

const char* asString(const Channel::Semantic semantic) {
  switch (semantic) {
    case Channel::Semantic::RED:
      return "R";
    case Channel::Semantic::GREEN:
      return "G";
    case Channel::Semantic::BLUE:
      return "B";
    case Channel::Semantic::ALPHA:
      return "A";
    default:
      return "#";
  }
}

const char* asString(const Channels::FormatType type) {
  switch (type) {
    case Channels::FormatType::UNSIGNED_NORMALIZED:
      return "";
    case Channels::FormatType::SIGNED_NORMALIZED:
      return "_SNORM";
    case Channels::FormatType::UNSIGNED_INTEGRAL:
      return "UI";
    case Channels::FormatType::SIGNED_INTEGRAL:
      return "I";
    case Channels::FormatType::FLOATING_POINT:
      return "F";
    default:
      return "#";
  }
}

}  // namespace

std::string Channels::asString() const {
  std::string result;
  for (const auto& channel : *this) {
    result += ::asString(channel.semantic);
    result += std::to_string(channel.bits);
  }
  result += ::asString(type);
  return result;
}
