#include <gtest/gtest.h>

#include "duke/shading/ShadingModel.hpp"
#include "duke/shading/ShadingUtils.hpp"

#include <iostream>
#include <deque>

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

TEST(ShadingModel, hash) {
  const std::string functionStr = R"(int foo(usampler2DRect sampler, vec2 offset) {
  return 1;
})";
  EXPECT_EQ(Function(functionStr).hash,Function(functionStr).hash);
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

Channels make(Channels::FormatType type, std::initializer_list<int> channels) {
  Channels value;
  std::deque<Channel::Semantic> semantics{Channel::Semantic::RED,  Channel::Semantic::GREEN,
                                          Channel::Semantic::BLUE, Channel::Semantic::ALPHA};
  for (const auto bits : channels) {
    value.emplace_back(semantics.front(), bits);
    semantics.pop_front();
  }
  value.type = type;
  return value;
}

TEST(ShadingUtils, needsUnpacking) {
  EXPECT_TRUE(needsUnpacking(make(Channels::FormatType::UNSIGNED_INTEGRAL, {10, 10, 10, 2})));
  EXPECT_TRUE(needsUnpacking(make(Channels::FormatType::UNSIGNED_NORMALIZED, {5, 5, 5, 1})));
  EXPECT_TRUE(needsUnpacking(make(Channels::FormatType::UNSIGNED_NORMALIZED, {5, 5, 5})));
  EXPECT_TRUE(needsUnpacking(make(Channels::FormatType::UNSIGNED_NORMALIZED, {2, 2, 2, 2})));

  EXPECT_FALSE(needsUnpacking(make(Channels::FormatType::UNSIGNED_NORMALIZED, {8, 8, 8})));
  EXPECT_FALSE(needsUnpacking(make(Channels::FormatType::UNSIGNED_INTEGRAL, {16, 16, 16})));
  EXPECT_FALSE(needsUnpacking(make(Channels::FormatType::FLOATING_POINT, {16, 16, 16, 16})));
}

Channels makeSemantic(const std::string semantics) {
  const auto getSemantic = [](char c) {
    switch (c) {
      case 'r':
        return Channel::Semantic::RED;
      case 'g':
        return Channel::Semantic::GREEN;
      case 'b':
        return Channel::Semantic::BLUE;
      case 'a':
        return Channel::Semantic::ALPHA;
      case 'z':
        return Channel::Semantic::DEPTH;
      default:
        return Channel::Semantic::UNKNOWN;
    }
  };
  Channels value;
  for (const char semantic : semantics) value.emplace_back(getSemantic(semantic), 8);
  return value;
}

TEST(ShadingUtils, isOrderedSemantic) {
  EXPECT_TRUE(isOrderedSemantic(makeSemantic("r")));
  EXPECT_TRUE(isOrderedSemantic(makeSemantic("rg")));
  EXPECT_TRUE(isOrderedSemantic(makeSemantic("rgb")));
  EXPECT_TRUE(isOrderedSemantic(makeSemantic("rgba")));

  EXPECT_FALSE(isOrderedSemantic(makeSemantic("")));
  EXPECT_FALSE(isOrderedSemantic(makeSemantic("rgbaz")));
  EXPECT_FALSE(isOrderedSemantic(makeSemantic("br")));
  EXPECT_FALSE(isOrderedSemantic(makeSemantic("bgra")));
}

bool operator==(const Channel &a, const Channel &b) { return a.semantic == b.semantic && a.bits == b.bits; }

TEST(ShadingUtils, getGlTextureRGB10A2) {
  const auto actual(getBestPackingTexture(make(Channels::FormatType::UNSIGNED_INTEGRAL, {10, 10, 10, 2})));
  const auto expected(make(Channels::FormatType::UNSIGNED_INTEGRAL, {8, 8, 8, 8}));
  EXPECT_EQ(expected, actual);
}

TEST(ShadingUtils, getGlTextureRGB5A1) {
  const auto actual(getBestPackingTexture(make(Channels::FormatType::UNSIGNED_INTEGRAL, {5, 5, 5, 1})));
  const auto expected(make(Channels::FormatType::UNSIGNED_INTEGRAL, {8, 8}));
  EXPECT_EQ(expected, actual);
}

TEST(ShadingUtils, getGlTextureRGB5) {
  const auto actual(getBestPackingTexture(make(Channels::FormatType::UNSIGNED_INTEGRAL, {5, 5, 5})));
  const auto expected(make(Channels::FormatType::UNSIGNED_INTEGRAL, {8, 8}));
  EXPECT_EQ(expected, actual);
}

TEST(ShadingUtils, getGlTextureRGBA2) {
  const auto actual(getBestPackingTexture(make(Channels::FormatType::UNSIGNED_NORMALIZED, {2, 2, 2, 2})));
  const auto expected(make(Channels::FormatType::UNSIGNED_INTEGRAL, {8}));
  EXPECT_EQ(expected, actual);
}

TEST(ShadingUtils, unpackRGB5) {
  const auto rg8 = make(Channels::FormatType::UNSIGNED_INTEGRAL, {8, 8});
  const auto rgb5 = make(Channels::FormatType::UNSIGNED_INTEGRAL, {5, 5, 5});
  const auto function = unpackToVec4(rg8, rgb5);
  const char expected[] = R"(vec4 sample_R8G8UI_as_R5G5B5UI(uvec2 input) {
  return vec4((((input.r & 0x7Cu) >> 2)) / 31.0f, (((input.g & 0xE0u) >> 5) | ((input.r & 0x3u) << 3)) / 31.0f, ((input.g & 0x1Fu)) / 31.0f, 1.0f);
})";
  EXPECT_EQ(expected, function.code);
}

TEST(ShadingUtils, unpackRGB10A2) {
  const auto rgba8 = make(Channels::FormatType::UNSIGNED_INTEGRAL, {8, 8, 8, 8});
  const auto rgb10a2 = make(Channels::FormatType::UNSIGNED_INTEGRAL, {10, 10, 10, 2});
  const auto function = unpackToVec4(rgba8, rgb10a2);
  const char expected[] = R"(vec4 sample_R8G8B8A8UI_as_R10G10B10A2UI(uvec4 input) {
  return vec4((((input.g & 0xC0u) >> 6) | (input.r << 2)) / 1023.0f, (((input.b & 0xF0u) >> 4) | ((input.g & 0x3Fu) << 4)) / 1023.0f, (((input.a & 0xFCu) >> 2) | ((input.b & 0xFu) << 6)) / 1023.0f, ((input.a & 0x3u)) / 3.0f);
})";
  EXPECT_EQ(expected, function.code);
}

TEST(ShadingUtils, RGBA8toVec4) {
  const auto rgba8 = make(Channels::FormatType::UNSIGNED_INTEGRAL, {8, 8, 8, 8});
  const auto function = asVec4(rgba8);
  const char expected[] = R"(vec4 sample_R8G8B8A8UI(uvec4 input) {
  return vec4((input.r) / 255.0f, (input.g) / 255.0f, (input.b) / 255.0f, (input.a) / 255.0f);
})";
  EXPECT_EQ(expected, function.code);
}

TEST(ShadingUtils, RGB8toVec4) {
  const auto rgb8 = make(Channels::FormatType::UNSIGNED_INTEGRAL, {8, 8, 8});
  const auto function = asVec4(rgb8);
  const char expected[] = R"(vec4 sample_R8G8B8UI(uvec3 input) {
  return vec4((input.r) / 255.0f, (input.g) / 255.0f, (input.b) / 255.0f, 1.0f);
})";
  EXPECT_EQ(expected, function.code);
}

std::string createCode(const std::vector<Function> &functions) {
  std::string code;
  for (const auto &function : functions) {
    for (const auto &parameter : function.parameters) {
      code += parameter;
      code += ";\n";
    }
    code += function.code;
    code += '\n';
  }
  return code;
}

TEST(ShadingUtils, sampleAsVecNoUnpacking) {
  const auto rgba8 = make(Channels::FormatType::UNSIGNED_INTEGRAL, {8, 8, 8, 8});
  Channels texture;
  std::vector<Function> functions;
  sampleAsVec4("foo", rgba8, texture, functions);
  EXPECT_EQ(rgba8, texture);
  EXPECT_EQ(3, functions.size());
  const auto code = createCode(functions);
  const auto expected = R"(uniform usampler2DRect gTextureSampler;
uvec4 sample(vec2 uv) {
  return texture(gTextureSampler, uv);
}
vec4 sample_R8G8B8A8UI(uvec4 input) {
  return vec4((input.r) / 255.0f, (input.g) / 255.0f, (input.b) / 255.0f, (input.a) / 255.0f);
}
vec4 foo(vec2 uv) {
  return sample_R8G8B8A8UI(sample(uv));
}
)";
  EXPECT_EQ(expected, code);
}

TEST(ShadingUtils, sampleAsVecUnpacking) {
  const auto rgb10a2 = make(Channels::FormatType::UNSIGNED_INTEGRAL, {10, 10, 10, 2});
  Channels texture;
  std::vector<Function> functions;
  sampleAsVec4("foo", rgb10a2, texture, functions);
  const auto rgba8 = make(Channels::FormatType::UNSIGNED_INTEGRAL, {8, 8, 8, 8});
  EXPECT_EQ(rgba8, texture);
  EXPECT_EQ(3, functions.size());
  const auto code = createCode(functions);
  const auto expected = R"(uniform usampler2DRect gTextureSampler;
uvec4 sample(vec2 uv) {
  return texture(gTextureSampler, uv);
}
vec4 sample_R8G8B8A8UI_as_R10G10B10A2UI(uvec4 input) {
  return vec4((((input.g & 0xC0u) >> 6) | (input.r << 2)) / 1023.0f, (((input.b & 0xF0u) >> 4) | ((input.g & 0x3Fu) << 4)) / 1023.0f, (((input.a & 0xFCu) >> 2) | ((input.b & 0xFu) << 6)) / 1023.0f, ((input.a & 0x3u)) / 3.0f);
}
vec4 foo(vec2 uv) {
  return sample_R8G8B8A8UI_as_R10G10B10A2UI(sample(uv));
}
)";
  EXPECT_EQ(expected, code);
}
