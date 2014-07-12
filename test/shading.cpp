#include <gtest/gtest.h>

#include "duke/shading/ShadingModel.hpp"
#include "duke/shading/ShadingUtils.hpp"

#include <iostream>

namespace duke {
namespace shader {

bool operator==(const Variable &a, const Variable &b) { return a.fulltype == b.fulltype && a.name == b.name; }

std::ostream &operator<<(std::ostream &stream, const Variable &a) { return stream << a.fulltype << ' ' << a.name; }

}  // namespace shader
}  // namespace duke

using namespace duke::shader;

TEST(ShadingModel, parse) {
  const std::string functionStr = R"(vec4 nearest(usampler2DRect sampler, vec2 offset) {
  return unpack(swizzle(texture(sampler, offset)));
})";
  const std::vector<std::string> parameters = {"smooth in vec2 vVaryingTexCoord",
                                               "uniform usampler2DRect gTextureSampler"};
  const Function function(functionStr, parameters);
  EXPECT_EQ(StringSlice("vec4"), function.returntype);
  EXPECT_EQ(StringSlice("nearest"), function.name);
  EXPECT_EQ(2, function.args.size());
  EXPECT_EQ(Variable("usampler2DRect", "sampler"), function.args.at(0));
  EXPECT_EQ(Variable("vec2", "offset"), function.args.at(1));
  EXPECT_EQ(2, function.params.size());
  EXPECT_EQ(Variable("smooth in vec2", "vVaryingTexCoord"), function.params.at(0));
  EXPECT_EQ(Variable("uniform usampler2DRect", "gTextureSampler"), function.params.at(1));
}

TEST(ShadingUtils, createAlphaSampler) {
  Channels channelsIn;
  channelsIn.type = Channels::FormatType::UNSIGNED_INTEGRAL;
  channelsIn.emplace_back(Channel::Semantic::ALPHA, 8);
  const auto function = createSampler("sample", channelsIn);
  EXPECT_EQ(StringSlice("sample"), function.name);
  EXPECT_EQ(StringSlice("uint"), function.returntype);
  EXPECT_EQ(R"(uniform usampler2DRect gTextureSampler)", function.parameters.at(0));
}

TEST(ShadingUtils, createRGBSampler) {
  Channels channelsIn;
  channelsIn.type = Channels::FormatType::FLOATING_POINT;
  channelsIn.emplace_back(Channel::Semantic::RED, 8);
  channelsIn.emplace_back(Channel::Semantic::GREEN, 8);
  channelsIn.emplace_back(Channel::Semantic::BLUE, 8);
  const auto function = createSampler("sample", channelsIn);
  EXPECT_EQ(StringSlice("sample"), function.name);
  EXPECT_EQ(StringSlice("vec3"), function.returntype);
  EXPECT_EQ(R"(uniform sampler2DRect gTextureSampler)", function.parameters.at(0));
}

TEST(ShadingUtils, adapterNoTransformationSingleChannel) {
  Channels channelsIn;
  channelsIn.type = Channels::FormatType::SIGNED_INTEGRAL;
  channelsIn.emplace_back(Channel::Semantic::RED, 8);
  const auto expected = R"(int f(int input) {
  return int(input);
})";
  EXPECT_EQ(expected, createChannelAdapter("f", channelsIn, channelsIn).function);
}

TEST(ShadingUtils, adapterNoTransformation) {
  Channels channelsIn;
  channelsIn.type = Channels::FormatType::SIGNED_INTEGRAL;
  channelsIn.emplace_back(Channel::Semantic::RED, 8);
  channelsIn.emplace_back(Channel::Semantic::GREEN, 8);
  channelsIn.emplace_back(Channel::Semantic::BLUE, 8);
  channelsIn.emplace_back(Channel::Semantic::ALPHA, 8);
  const auto expected = R"(ivec4 f(ivec4 input) {
  return ivec4(input.r, input.g, input.b, input.a);
})";
  EXPECT_EQ(expected, createChannelAdapter("f", channelsIn, channelsIn).function);
}

TEST(ShadingUtils, adapterAlpha) {
  Channels channelsIn;
  channelsIn.type = Channels::FormatType::SIGNED_INTEGRAL;
  channelsIn.emplace_back(Channel::Semantic::ALPHA, 8);
  Channels channelsOut;
  channelsOut.type = Channels::FormatType::SIGNED_INTEGRAL;
  channelsOut.emplace_back(Channel::Semantic::RED, 8);
  channelsOut.emplace_back(Channel::Semantic::GREEN, 8);
  channelsOut.emplace_back(Channel::Semantic::BLUE, 8);
  channelsOut.emplace_back(Channel::Semantic::ALPHA, 8);
  const auto expected = R"(ivec4 f(int input) {
  return ivec4(0, 0, 0, input);
})";
  EXPECT_EQ(expected, createChannelAdapter("f", channelsIn, channelsOut).function);
}

TEST(ShadingUtils, adapterSwizzle) {
  Channels channelsIn;
  channelsIn.type = Channels::FormatType::FLOATING_POINT;
  channelsIn.emplace_back(Channel::Semantic::BLUE, 8);
  channelsIn.emplace_back(Channel::Semantic::GREEN, 8);
  channelsIn.emplace_back(Channel::Semantic::RED, 8);
  Channels channelsOut;
  channelsOut.type = Channels::FormatType::FLOATING_POINT;
  channelsOut.emplace_back(Channel::Semantic::RED, 8);
  channelsOut.emplace_back(Channel::Semantic::GREEN, 8);
  channelsOut.emplace_back(Channel::Semantic::BLUE, 8);
  channelsOut.emplace_back(Channel::Semantic::ALPHA, 8);
  const auto expected = R"(vec4 f(vec3 input) {
  return vec4(input.b, input.g, input.r, 1.0);
})";
  EXPECT_EQ(expected, createChannelAdapter("f", channelsIn, channelsOut).function);
}

TEST(ShadingUtils, adapterDynamic) {
  Channels channelsIn;
  channelsIn.type = Channels::FormatType::SIGNED_INTEGRAL;
  channelsIn.emplace_back(Channel::Semantic::RED, 8);
  Channels channelsOut;
  channelsOut.type = Channels::FormatType::FLOATING_POINT;
  channelsOut.emplace_back(Channel::Semantic::RED, 16);
  const auto expected = R"(float f(int input) {
  return float(input * 1.0 / 255);
})";
  EXPECT_EQ(expected, createChannelAdapter("f", channelsIn, channelsOut).function);
}

TEST(ShadingUtils, adapterAll) {
  Channels channelsIn;
  channelsIn.type = Channels::FormatType::SIGNED_INTEGRAL;
  channelsIn.emplace_back(Channel::Semantic::BLUE, 16);
  channelsIn.emplace_back(Channel::Semantic::RED, 16);
  Channels channelsOut;
  channelsOut.type = Channels::FormatType::UNSIGNED_INTEGRAL;
  channelsOut.emplace_back(Channel::Semantic::RED, 8);
  channelsOut.emplace_back(Channel::Semantic::GREEN, 8);
  channelsOut.emplace_back(Channel::Semantic::BLUE, 8);
  channelsOut.emplace_back(Channel::Semantic::ALPHA, 8);
  const auto expected = R"(uvec4 f(ivec2 input) {
  return uvec4(input.g * 255 / 65535, 0, input.r * 255 / 65535, 255);
})";
  EXPECT_EQ(expected, createChannelAdapter("f", channelsIn, channelsOut).function);
}
