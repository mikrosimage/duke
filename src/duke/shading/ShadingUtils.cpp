#include "duke/base/StringAppender.hpp"
#include "duke/base/StringTemplate.hpp"
#include "duke/shading/ShadingUtils.hpp"

#include <cmath>

namespace duke {
namespace shader {

namespace {

bool isFloatingPoint(Channels::FormatType type) {
  switch (type) {
    case Channels::FormatType::FLOATING_POINT:
    case Channels::FormatType::UNSIGNED_NORMALIZED:
    case Channels::FormatType::SIGNED_NORMALIZED:
      return true;
    default:
      return false;
  }
}

StringSlice getTypePrefix(Channels::FormatType type) {
  switch (type) {
    case Channels::FormatType::UNSIGNED_INTEGRAL:
      return "u";
    case Channels::FormatType::SIGNED_INTEGRAL:
      return "i";
    case Channels::FormatType::FLOATING_POINT:
    case Channels::FormatType::UNSIGNED_NORMALIZED:
    case Channels::FormatType::SIGNED_NORMALIZED:
      return "";
    default:
      CHECK(false) << "unknown type";
      return {};
  }
}

std::string getType(const Channels& channels) {
  const auto channelCount = channels.size();
  switch (channelCount) {
    default:
      CHECK(channelCount <= 4) << "Can't handle more than 4 channels";
      return {};
    case 1:
      switch (channels.type) {
        case Channels::FormatType::UNSIGNED_INTEGRAL:
          return "uint";
        case Channels::FormatType::SIGNED_INTEGRAL:
          return "int";
        case Channels::FormatType::FLOATING_POINT:
        case Channels::FormatType::UNSIGNED_NORMALIZED:
        case Channels::FormatType::SIGNED_NORMALIZED:
          return "float";
        default:
          CHECK(false) << "unknown type";
          return {};
      }
      break;
    case 2:
    case 3:
    case 4: {
      std::string value = getTypePrefix(channels.type).ToString();
      value += "vec";
      value += '0' + channelCount;
      return value;
    }
  }
}

std::string getMaxValue(uint8_t bits, Channels::FormatType type) {
  return isFloatingPoint(type) ? "1.0" : std::to_string((size_t)std::pow(2, bits) - 1);
};

std::string getDefaultValue(const Channel& channel, Channels::FormatType type) {
  return channel.semantic == Channel::Semantic::ALPHA ? getMaxValue(channel.bits, type) : "0";
};

void appendSampler(const StringSlice prefix, StringAppender& output) {
  static const StringTemplate tmpl(R"(uniform #prefix#sampler2DRect gTextureSampler)");
  tmpl.instantiate({{"prefix", prefix}}, output);
  CHECK(output);
}

void appendFunction(const StringSlice returnType, const StringSlice name, StringAppender& output) {
  static const StringTemplate tmpl(R"(#returnType# #name#(vec2 uv) {
  return texture(gTextureSampler, uv);
})");
  tmpl.instantiate({{"returnType", returnType}, {"name", name}, }, output);
  CHECK(output);
}

void appendDynamicAdapterExpression(const StringSlice input, uint8_t inBits, Channels::FormatType inType,
                                    uint8_t outBits, Channels::FormatType outType, StringAppender& output) {
  const bool sameBits = inBits == outBits;
  const bool sameType = inType == outType || (isFloatingPoint(inType) && isFloatingPoint(outType));
  if (sameBits && sameType) {
    output.append(input);
    return;
  }
  const std::string maxInValue = getMaxValue(inBits, inType);
  const std::string maxOutValue = getMaxValue(outBits, outType);
  static const StringTemplate tmpl(R"(#input# * #maxOut# / #maxIn#)");
  tmpl.instantiate({{"input", input}, {"maxIn", maxInValue}, {"maxOut", maxOutValue}}, output);
}

void appendOutTypeConstructorArguments(const Channels& channelsIn, const Channels& channelsOut,
                                       StringAppender& output) {
  const auto FindInChannelWithSemanticIndex = [&channelsIn](Channel::Semantic semantic) {
    for (size_t i = 0; i < channelsIn.size(); ++i)
      if (channelsIn[i].semantic == semantic) return i;
    return std::string::npos;
  };
  for (size_t i = 0; i < channelsOut.size(); ++i) {
    const auto& channelOut = channelsOut[i];
    if (i > 0) output.append(", ");
    const auto inChannelIndex = FindInChannelWithSemanticIndex(channelOut.semantic);
    if (inChannelIndex == std::string::npos) {
      output.append(getDefaultValue(channelOut, channelsOut.type));
    } else {
      CHECK(inChannelIndex >= 0 && inChannelIndex <= 3);
      const Channel& channelIn = channelsIn[inChannelIndex];
      static const std::array<const char*, 4> inputs = {"input.r", "input.g", "input.b", "input.a"};
      const char* const pInput = channelsIn.size() == 1 ? "input" : inputs.at(inChannelIndex);
      appendDynamicAdapterExpression(pInput, channelIn.bits, channelsIn.type,  //
                                     channelOut.bits, channelsOut.type, output);
    }
  }
}

}  // namespace

Function createSampler(const StringSlice name, const Channels& channels) {
  const auto prefix = getTypePrefix(channels.type);
  const auto type = getType(channels);
  BufferStringAppender<128> samplerBuffer;
  appendSampler(prefix, samplerBuffer);
  BufferStringAppender<512> functionBuffer;
  appendFunction(type, name, functionBuffer);
  return Function(functionBuffer.c_str(), {samplerBuffer.c_str()});
}

Function createChannelAdapter(const StringSlice name, const Channels& channelsIn, const Channels& channelsOut) {
  BufferStringAppender<1024> buffer;
  appendOutTypeConstructorArguments(channelsIn, channelsOut, buffer);
  BufferStringAppender<1024> functionBuffer;
  const auto outType = getType(channelsOut);
  const auto inType = getType(channelsIn);
  static const StringTemplate tmpl(R"(#outType# #name#(#inType# input) {
  return #outType#(#outTypeCtorArgs#);
})");
  tmpl.instantiate({{"inType", inType}, {"outType", outType}, {"name", name}, {"outTypeCtorArgs", buffer.c_str()}},
                   functionBuffer);
  CHECK(functionBuffer);
  return Function(functionBuffer.c_str());
}

} /* namespace shader */
} /* namespace duke */
