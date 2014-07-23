#include "duke/base/StringAppender.hpp"
#include "duke/base/StringTemplate.hpp"
#include "duke/gl/GlUtils.hpp"
#include "duke/gl/GL.hpp"
#include "duke/shading/ShadingUtils.hpp"

#include <array>
#include <bitset>
#include <stack>
#include <type_traits>

#include <cmath>

namespace duke {
namespace shader {

namespace {

template <typename E>
typename std::underlying_type<E>::type to_underlying(E e) {
  return static_cast<typename std::underlying_type<E>::type>(e);
}

void appendSemantic(const Channel::Semantic semantic, StringAppender& output) {
  static const std::array<char, 6> letters = {'X', 'R', 'G', 'B', 'A', 'Z'};
  output.append(letters.at(to_underlying(semantic)));
}

void appendChannel(const Channel& channel, StringAppender& output) {
  appendSemantic(channel.semantic, output);
  output.appendInteger(channel.bits, 10);
}

void appendChannels(const Channels& channels, StringAppender& output) {
  for (const auto& channel : channels) appendChannel(channel, output);
  static const std::array<StringSlice, 6> suffix = {"UNKNOWN", "", "SNORM", "UI", "I", "F"};
  output.append(suffix.at(to_underlying(channels.type)));
}

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

uint64_t getMaxValue(uint8_t bits, Channels::FormatType type) {
  return isFloatingPoint(type) ? 1ull
                               : (type == Channels::FormatType::SIGNED_INTEGRAL ? (uint64_t)(std::pow(2, bits - 1) - 1)
                                                                                : (uint64_t)(std::pow(2, bits) - 1));
}

uint64_t getDefaultValue(const Channel& channel, Channels::FormatType type) {
  return channel.semantic == Channel::Semantic::ALPHA ? getMaxValue(channel.bits, type) : 0;
}

void appendDefaultValue(const Channel& channel, Channels::FormatType type, StringAppender& output) {
  output.appendInteger(getDefaultValue(channel, type));
  if (type == Channels::FormatType::FLOATING_POINT) output.append(".0f");
}

// void appendDynamicAdapterExpression(const StringSlice input, uint8_t inBits, Channels::FormatType inType,
//                                    uint8_t outBits, Channels::FormatType outType, StringAppender& output) {
//  const bool sameBits = inBits == outBits;
//  const bool sameType = inType == outType || (isFloatingPoint(inType) && isFloatingPoint(outType));
//  if (sameBits && sameType) {
//    output.append(input);
//    return;
//  }
//  const std::string maxInValue = getMaxValue(inBits, inType);
//  const std::string maxOutValue = getMaxValue(outBits, outType);
//  static const StringTemplate tmpl(R"((#input#) * #maxOut# / #maxIn#)");
//  tmpl.instantiate({{"input", input}, {"maxIn", maxInValue}, {"maxOut", maxOutValue}}, output);
//}

uint32_t getMask(uint8_t from, uint8_t to) {
  std::bitset<32> mask;
  CHECK(from < mask.size() && to < mask.size());
  for (uint8_t i = from; i < to; ++i) mask.set(i);
  return mask.to_ulong();
}

struct Operation {
  uint8_t inIndex;
  uint8_t inOffset;
  uint8_t outIndex;
  uint8_t outOffset;
  uint8_t count;
  Operation(uint8_t inIndex, uint8_t inOffset, uint8_t outIndex, uint8_t outOffset, uint8_t count)
      : inIndex(inIndex), inOffset(inOffset), outIndex(outIndex), outOffset(outOffset), count(count) {}
};

void setChannelsShiftAndMaskOperations(const Channels& storageChannels, const Channels& packedAsChannels,
                                       std::vector<Operation>(&output)[4]) {
  std::stack<uint8_t> in;
  for (const auto& channel : storageChannels) in.push(channel.bits);
  std::stack<uint8_t> out;
  for (const auto& channel : packedAsChannels) out.push(channel.bits);

  uint8_t in_offset = 0;
  uint8_t out_offset = 0;
  uint8_t in_index = storageChannels.size() - 1;
  uint8_t out_index = packedAsChannels.size() - 1;
  while (!out.empty() && !in.empty()) {
    const auto in_bits = in.top();
    const auto out_bits = out.top();
    const auto in_available_bits = in_bits - in_offset;
    const auto out_available_bits = out_bits - out_offset;
    const auto available_bits = std::min(in_available_bits, out_available_bits);
    output[out_index].emplace_back(in_index, in_offset, out_index, out_offset, available_bits);
    if (in_available_bits > out_available_bits) {
      in_offset += available_bits;
      out_offset = 0;
      out.pop();
      --out_index;
    } else if (in_available_bits < out_available_bits) {
      in_offset = 0;
      out_offset += available_bits;
      in.pop();
      --in_index;
    } else {
      in_offset = 0;
      out_offset = 0;
      in.pop();
      out.pop();
      --in_index;
      --out_index;
    }
  }
}

struct ChannelAdapter {
  ChannelAdapter(const StringSlice input, const Channels& storage, const Channels& packedAs)
      : m_InputName(input), m_StorageChannels(storage), m_PackedAsChannels(packedAs) {
    CHECK(m_StorageChannels.size() <= 4 && m_PackedAsChannels.size() <= 4);
    setChannelsShiftAndMaskOperations(m_StorageChannels, m_PackedAsChannels, m_ChannelShiftAndMaskOperations);
    for (size_t packedChannelsIndex = 0; packedChannelsIndex < m_PackedAsChannels.size(); ++packedChannelsIndex)
      setChannelArg(packedChannelsIndex, m_ChannelExpressions.at(packedChannelsIndex));
    static const Channels outputChannels =
        Channels(Channels::FormatType::FLOATING_POINT,
                 {Channel(Channel::Semantic::RED, 16),  Channel(Channel::Semantic::GREEN, 16),
                  Channel(Channel::Semantic::BLUE, 16), Channel(Channel::Semantic::ALPHA, 16), });
    auto expressionItr = m_ChannelExpressions.begin();
    for (const auto& outChannel : outputChannels) {
      if (!m_OutTypeCtorArgs.empty()) m_OutTypeCtorArgs.append(", ");
      if (expressionItr->empty()) {
        appendDefaultValue(outChannel, outputChannels.type, m_OutTypeCtorArgs);
      } else {
        m_OutTypeCtorArgs.append(expressionItr->slice());
      }
      ++expressionItr;
    }
  }

  void setChannelArg(const size_t packedChannelsIndex, StringAppender& output) const {
    static const std::array<unsigned char, 4> letters = {'r', 'g', 'b', 'a'};
    if (m_StorageChannels.size() == 1) {
      output.append(m_InputName);
      return;
    }
    output.append('(');
    bool orExpression = false;
    for (const auto& op : m_ChannelShiftAndMaskOperations[packedChannelsIndex]) {
      if (orExpression) output.append(" | ");
      orExpression = true;
      const bool maskNotNeeded = op.inOffset == 0 && op.count == m_StorageChannels[op.inIndex].bits;
      const int shift = (int)(op.outOffset) - op.inOffset;
      if (shift) output.append('(');
      if (maskNotNeeded) {
        output.append(m_InputName);
        output.append('.');
        output.append(letters.at(op.inIndex));
      } else {
        const auto mask = getMask(op.inOffset, op.inOffset + op.count);
        output.append('(');
        output.append(m_InputName);
        output.append('.');
        output.append(letters.at(op.inIndex));
        output.append(" & 0x");
        output.appendInteger(mask, 16);
        output.append("u)");
      }
      if (shift) {
        output.append(shift > 0 ? " << " : " >> ");
        output.appendInteger(abs(shift));
        output.append(')');
      }
    }
    output.append(") / ");
    const auto& channelOut = m_PackedAsChannels.at(packedChannelsIndex);
    output.appendInteger(getMaxValue(channelOut.bits, m_PackedAsChannels.type));
    output.append(".0f");
  }

  Function getFunction(StringSlice name) const {
    const auto inType = getType(m_StorageChannels);
    BufferStringAppender<1024> buffer;
    static const StringTemplate tmpl(R"(vec4 #name#(#inType# #input#) {
  return vec4(#outTypeCtorArgs#);
})");
    tmpl.instantiate(
        {{"inType", inType}, {"input", m_InputName}, {"name", name}, {"outTypeCtorArgs", m_OutTypeCtorArgs.slice()}},
        buffer);
    CHECK(buffer);
    return Function(buffer.slice());
  }

 private:
  const StringSlice m_InputName;
  const Channels& m_StorageChannels;
  const Channels& m_PackedAsChannels;
  std::vector<Operation> m_ChannelShiftAndMaskOperations[4];
  std::array<BufferStringAppender<64>, 4> m_ChannelExpressions;
  BufferStringAppender<1024> m_OutTypeCtorArgs;
};

}  // namespace

Function createSampler(const StringSlice name, const Channels& channels) {
  const auto prefix = getTypePrefix(channels.type);
  const auto type = getType(channels);
  BufferStringAppender<128> samplerBuffer;
  {
    static const StringTemplate tmpl(R"(uniform #prefix#sampler2DRect gTextureSampler)");
    tmpl.instantiate({{"prefix", prefix}}, samplerBuffer);
    CHECK(samplerBuffer);
  }
  BufferStringAppender<512> functionBuffer;
  {
    static const StringTemplate tmpl(R"(#returnType# #name#(vec2 uv) {
  return texture(gTextureSampler, uv);
})");
    tmpl.instantiate({{"returnType", type}, {"name", name}, }, functionBuffer);
    CHECK(functionBuffer);
  }
  return Function(functionBuffer.c_str(), {samplerBuffer.c_str()});
}

bool needsUnpacking(const Channels& channels) {
  for (const auto& channel : channels) {
    switch (channel.bits) {
      case 8:
      case 16:
      case 32:
        continue;
      default:
        return true;
    }
  }
  return false;
}

bool isOrderedSemantic(const Channels& channels) {
  const auto count = channels.size();
  if (count == 0 || count > 4) return false;
  const bool isR = channels.at(0).semantic == Channel::Semantic::RED;
  if (count == 1) return isR;
  const bool isG = channels.at(1).semantic == Channel::Semantic::GREEN;
  if (count == 2) return isR && isG;
  const bool isB = channels.at(2).semantic == Channel::Semantic::BLUE;
  if (count == 3) return isR && isG && isB;
  const bool isA = channels.at(3).semantic == Channel::Semantic::ALPHA;
  if (count == 4) return isR && isG && isB && isA;
  return true;
}

Channels getBestPackingTexture(const Channels& channels) {
  size_t all_bits = 0;
  for (const auto& channel : channels) {
    all_bits += channel.bits;
  }
  if (all_bits <= 8)
    return getChannels(GL_R8UI);
  else if (all_bits <= 16)
    return getChannels(GL_RG8UI);
  else if (all_bits <= 24)
    return getChannels(GL_RGB8UI);
  else if (all_bits <= 32)
    return getChannels(GL_RGBA8UI);
  CHECK(false) << "Packed texture is not a multiple of 8 bits and not mappable to a GL texture";
  return {};
}

Function unpackToVec4(const Channels& storage, const Channels& packedAs) {
  CHECK(isOrderedSemantic(storage) && isOrderedSemantic(packedAs));
  BufferStringAppender<64> functionName;
  functionName.append("sample_");
  appendChannels(storage, functionName);
  functionName.append("_as_");
  appendChannels(packedAs, functionName);
  return ChannelAdapter("input", storage, packedAs).getFunction(functionName.slice());
}

Function asVec4(const Channels& input) {
  CHECK(isOrderedSemantic(input));
  BufferStringAppender<64> functionName;
  functionName.append("sample_");
  appendChannels(input, functionName);
  return ChannelAdapter("input", input, input).getFunction(functionName.slice());
}

void sampleAsVec4(const StringSlice sampleFunctionName, const Channels& input, Channels& texture,
                  std::vector<Function>& functions) {
  CHECK(isOrderedSemantic(input)) << "can take only ordered semantic for now";
  functions.clear();
  functions.push_back(createSampler("sample", input));
  if (needsUnpacking(input)) {
    texture = getBestPackingTexture(input);
    functions.push_back(unpackToVec4(texture, input));
  } else {
    texture = input;
    functions.push_back(asVec4(input));
  }
  BufferStringAppender<256> calls;
  for (auto itr = functions.rbegin(); itr != functions.rend(); ++itr) {
    calls.append(itr->name);
    calls.append('(');
  }
  calls.append("uv");
  for (const auto& function : functions) {
    calls.append(')');
  }
  BufferStringAppender<256> main;
  {
    static const StringTemplate tmpl(R"(vec4 #name#(vec2 uv) {
  return #calls#;
})");
    CHECK(calls);
    tmpl.instantiate({{"name", sampleFunctionName}, {"calls", calls.slice()}}, main);
    CHECK(main);
  }
  functions.emplace_back(main.slice());
}

} /* namespace shader */
} /* namespace duke */
