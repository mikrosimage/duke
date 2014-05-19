#include "GlUtils.hpp"

#include <duke/base/Check.hpp>
#include <duke/base/StringUtils.hpp>
#include <duke/gl/GL.hpp>

#include <fstream>
#include <map>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

bool operator<(const Channel& a, const Channel& b) {
  return std::tie(a.semantic, a.bits) < std::tie(b.semantic, b.bits);
}

namespace {

Channel::Semantic getSemantic(const char c) {
  switch (c) {
    case 'R':
      return Channel::Semantic::RED;
    case 'G':
      return Channel::Semantic::GREEN;
    case 'B':
      return Channel::Semantic::BLUE;
    case 'A':
      return Channel::Semantic::ALPHA;
    default:
      CHECK(false) << "Invalid semantic";
      return Channel::Semantic::UNKNOWN;
  }
}

void appendChannel(const std::csub_match& semantic_, const std::csub_match& bits_, Channels& channels) {
  if (!(semantic_.matched && bits_.matched)) return;
  const uint8_t bits = std::stoi(bits_);
  for (const char c : semantic_.str()) channels.emplace_back(getSemantic(c), bits);
}

Channels::FormatType parseType(const std::string& string) {
  if (string.empty()) return Channels::FormatType::UNSIGNED_NORMALIZED;
  if (string == "F") return Channels::FormatType::FLOATING_POINT;
  if (string == "UI") return Channels::FormatType::UNSIGNED_INTEGRAL;
  if (string == "I") return Channels::FormatType::SIGNED_INTEGRAL;
  if (string == "_SNORM") return Channels::FormatType::SIGNED_NORMALIZED;
  CHECK(false) << "Invalid format type '" << string << "'";
  return Channels::FormatType::UNKNOWN;
}

Channels parseOpenGlFormat(const char* ogl) {
  static const std::regex regex(
      R"(^GL_(?:(R|RG|RGB|RGBA)(\d{1,2}))(?:_(A|G|B)(\d{1,2}))?(?:_(A|B)(\d{1,2}))?(_SNORM|UI|I|F)?$)");
  std::cmatch m;
  std::regex_match(ogl, m, regex);
  CHECK(!m.empty()) << "Can't match '" << ogl << "'";
  Channels channels;
  appendChannel(m[1], m[2], channels);
  appendChannel(m[3], m[4], channels);
  appendChannel(m[5], m[6], channels);
  channels.type = parseType(m[7].str());
  return channels;
}

struct ChannelComparator : public std::binary_function<bool, Channels, Channels> {
  bool operator()(const Channels& a, const Channels& b) const {
    if (a.type == b.type)
      return static_cast<const std::vector<Channel>&>(a) < static_cast<const std::vector<Channel>&>(b);
    return a.type < b.type;
  }
};
}

Channels getChannels(int32_t internalFormat) { return parseOpenGlFormat(getInternalFormatString(internalFormat)); }

int32_t getOpenGlFormat(const Channels& channels) {
#define MAKE_PAIR(X) std::make_pair(getChannels(X), X)
  static const std::map<Channels, int32_t, ChannelComparator> kStaticMap{
      MAKE_PAIR(GL_R16),          MAKE_PAIR(GL_R16F),        MAKE_PAIR(GL_R16I),       MAKE_PAIR(GL_R16_SNORM),
      MAKE_PAIR(GL_R16UI),        MAKE_PAIR(GL_R32F),        MAKE_PAIR(GL_R32I),       MAKE_PAIR(GL_R32UI),
      MAKE_PAIR(GL_R3_G3_B2),     MAKE_PAIR(GL_R8),          MAKE_PAIR(GL_R8I),        MAKE_PAIR(GL_R8_SNORM),
      MAKE_PAIR(GL_R8UI),         MAKE_PAIR(GL_RG16),        MAKE_PAIR(GL_RG16F),      MAKE_PAIR(GL_RG16I),
      MAKE_PAIR(GL_RG16_SNORM),   MAKE_PAIR(GL_RG16UI),      MAKE_PAIR(GL_RG32F),      MAKE_PAIR(GL_RG32I),
      MAKE_PAIR(GL_RG32UI),       MAKE_PAIR(GL_RG8),         MAKE_PAIR(GL_RG8I),       MAKE_PAIR(GL_RG8_SNORM),
      MAKE_PAIR(GL_RG8UI),        MAKE_PAIR(GL_RGB10),       MAKE_PAIR(GL_RGB10_A2),   MAKE_PAIR(GL_RGB10_A2UI),
      MAKE_PAIR(GL_RGB12),        MAKE_PAIR(GL_RGB16),       MAKE_PAIR(GL_RGB16F),     MAKE_PAIR(GL_RGB16I),
      MAKE_PAIR(GL_RGB16_SNORM),  MAKE_PAIR(GL_RGB16UI),     MAKE_PAIR(GL_RGB32F),     MAKE_PAIR(GL_RGB32I),
      MAKE_PAIR(GL_RGB32UI),      MAKE_PAIR(GL_RGB4),        MAKE_PAIR(GL_RGB5),       MAKE_PAIR(GL_RGB5_A1),
      MAKE_PAIR(GL_RGB8),         MAKE_PAIR(GL_RGB8I),       MAKE_PAIR(GL_RGB8_SNORM), MAKE_PAIR(GL_RGB8UI),
      MAKE_PAIR(GL_RGBA12),       MAKE_PAIR(GL_RGBA16),      MAKE_PAIR(GL_RGBA16F),    MAKE_PAIR(GL_RGBA16I),
      MAKE_PAIR(GL_RGBA16_SNORM), MAKE_PAIR(GL_RGBA16UI),    MAKE_PAIR(GL_RGBA2),      MAKE_PAIR(GL_RGBA32F),
      MAKE_PAIR(GL_RGBA32I),      MAKE_PAIR(GL_RGBA32UI),    MAKE_PAIR(GL_RGBA4),      MAKE_PAIR(GL_RGBA8),
      MAKE_PAIR(GL_RGBA8I),       MAKE_PAIR(GL_RGBA8_SNORM), MAKE_PAIR(GL_RGBA8UI), };
#undef MAKE_PAIR
  const auto pFound = kStaticMap.find(channels);
  if (pFound == kStaticMap.end()) return 0;
  return pFound->second;
}

const char* getInternalFormatString(GLint internalFormat) {
#define CASE(X) \
  case X:       \
    return #X

  switch (internalFormat) {
    CASE(GL_R16);
    CASE(GL_R16F);
    CASE(GL_R16I);
    CASE(GL_R16_SNORM);
    CASE(GL_R16UI);
    CASE(GL_R32F);
    CASE(GL_R32I);
    CASE(GL_R32UI);
    CASE(GL_R3_G3_B2);
    CASE(GL_R8);
    CASE(GL_R8I);
    CASE(GL_R8_SNORM);
    CASE(GL_R8UI);
    CASE(GL_RG16);
    CASE(GL_RG16F);
    CASE(GL_RG16I);
    CASE(GL_RG16_SNORM);
    CASE(GL_RG16UI);
    CASE(GL_RG32F);
    CASE(GL_RG32I);
    CASE(GL_RG32UI);
    CASE(GL_RG8);
    CASE(GL_RG8I);
    CASE(GL_RG8_SNORM);
    CASE(GL_RG8UI);
    CASE(GL_RGB10);
    CASE(GL_RGB10_A2);
    CASE(GL_RGB10_A2UI);
    CASE(GL_RGB12);
    CASE(GL_RGB16);
    CASE(GL_RGB16F);
    CASE(GL_RGB16I);
    CASE(GL_RGB16_SNORM);
    CASE(GL_RGB16UI);
    CASE(GL_RGB32F);
    CASE(GL_RGB32I);
    CASE(GL_RGB32UI);
    CASE(GL_RGB4);
    CASE(GL_RGB5);
    CASE(GL_RGB5_A1);
    CASE(GL_RGB8);
    CASE(GL_RGB8I);
    CASE(GL_RGB8_SNORM);
    CASE(GL_RGB8UI);
    CASE(GL_RGBA12);
    CASE(GL_RGBA16);
    CASE(GL_RGBA16F);
    CASE(GL_RGBA16I);
    CASE(GL_RGBA16_SNORM);
    CASE(GL_RGBA16UI);
    CASE(GL_RGBA2);
    CASE(GL_RGBA32F);
    CASE(GL_RGBA32I);
    CASE(GL_RGBA32UI);
    CASE(GL_RGBA4);
    CASE(GL_RGBA8);
    CASE(GL_RGBA8I);
    CASE(GL_RGBA8_SNORM);
    CASE(GL_RGBA8UI);
  }

#undef CASE
  CHECK(false) << "Unknown code 0x" << std::hex << internalFormat;
  return "Unknown";
}

const char* getPixelFormatString(unsigned int pixelFormat) {
  switch (pixelFormat) {
    case GL_RED:
      return "GL_RED";
    case GL_RG:
      return "GL_RG";
    case GL_RGB:
      return "GL_RGB";
    case GL_BGR:
      return "GL_BGR";
    case GL_RGBA:
      return "GL_RGBA";
    case GL_BGRA:
      return "GL_BGRA";
    case GL_RED_INTEGER:
      return "GL_RED_INTEGER";
    case GL_RG_INTEGER:
      return "GL_RG_INTEGER";
    case GL_RGB_INTEGER:
      return "GL_RGB_INTEGER";
    case GL_BGR_INTEGER:
      return "GL_BGR_INTEGER";
    case GL_RGBA_INTEGER:
      return "GL_RGBA_INTEGER";
    case GL_BGRA_INTEGER:
      return "GL_BGRA_INTEGER";
  }
  return "Unknown";
}

const char* getPixelTypeString(unsigned int pixelType) {
  switch (pixelType) {
    case GL_UNSIGNED_BYTE:
      return "GL_UNSIGNED_BYTE";
    case GL_BYTE:
      return "GL_BYTE";
    case GL_UNSIGNED_SHORT:
      return "GL_UNSIGNED_SHORT";
    case GL_SHORT:
      return "GL_SHORT";
    case GL_UNSIGNED_INT:
      return "GL_UNSIGNED_INT";
    case GL_INT:
      return "GL_INT";
    case GL_HALF_FLOAT:
      return "GL_HALF_FLOAT";
    case GL_FLOAT:
      return "GL_FLOAT";
    case GL_UNSIGNED_BYTE_3_3_2:
      return "GL_UNSIGNED_BYTE_3_3_2";
    case GL_UNSIGNED_BYTE_2_3_3_REV:
      return "GL_UNSIGNED_BYTE_2_3_3_REV";
    case GL_UNSIGNED_SHORT_5_6_5:
      return "GL_UNSIGNED_SHORT_5_6_5";
    case GL_UNSIGNED_SHORT_5_6_5_REV:
      return "GL_UNSIGNED_SHORT_5_6_5_REV";
    case GL_UNSIGNED_SHORT_4_4_4_4:
      return "GL_UNSIGNED_SHORT_4_4_4_4";
    case GL_UNSIGNED_SHORT_4_4_4_4_REV:
      return "GL_UNSIGNED_SHORT_4_4_4_4_REV";
    case GL_UNSIGNED_SHORT_5_5_5_1:
      return "GL_UNSIGNED_SHORT_5_5_5_1";
    case GL_UNSIGNED_SHORT_1_5_5_5_REV:
      return "GL_UNSIGNED_SHORT_1_5_5_5_REV";
    case GL_UNSIGNED_INT_8_8_8_8:
      return "GL_UNSIGNED_INT_8_8_8_8";
    case GL_UNSIGNED_INT_8_8_8_8_REV:
      return "GL_UNSIGNED_INT_8_8_8_8_REV";
    case GL_UNSIGNED_INT_10_10_10_2:
      return "GL_UNSIGNED_INT_10_10_10_2";
    case GL_UNSIGNED_INT_2_10_10_10_REV:
      return "GL_UNSIGNED_INT_2_10_10_10_REV";
  }
  return "Unknown";
}

namespace {

#ifndef NDEBUG
const char* getErrorString(unsigned error) {
  switch (error) {
    case GL_INVALID_ENUM:
      return "Invalid enum";
    case GL_INVALID_VALUE:
      return "Invalid value";
    case GL_INVALID_OPERATION:
      return "Invalid operation";
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      return "Invalid framebuffer operation";
    case GL_OUT_OF_MEMORY:
      return "Out of memory";
      // case GL_STACK_UNDERFLOW:
      //	throw std::runtime_error("OpenGL : Stack underflow");
      // case GL_STACK_OVERFLOW:
      //	throw std::runtime_error("OpenGL : Stack overflow");
  }
  return "Unknown error";
}
#endif

}  // namespace

void glCheckError() {
#ifndef NDEBUG
  std::vector<unsigned> errors;
  unsigned error = GL_NO_ERROR;
  for (; (error = glGetError()) != GL_NO_ERROR;) errors.push_back(error);
  if (errors.empty()) return;
  std::ostringstream oss;
  oss << "OpenGL errors :\n";
  for (const unsigned error : errors) oss << " - " << getErrorString(error) << '\n';
  throw std::runtime_error(oss.str());
#endif
}

namespace {

GLuint getBindParameter(GLuint targetType) {
  switch (targetType) {
    case GL_TEXTURE_2D:
      return GL_TEXTURE_BINDING_2D;
    case GL_TEXTURE_RECTANGLE:
      return GL_TEXTURE_BINDING_RECTANGLE;
    case GL_ARRAY_BUFFER:
      return GL_ARRAY_BUFFER_BINDING;
    case GL_ELEMENT_ARRAY_BUFFER:
      return GL_ELEMENT_ARRAY_BUFFER_BINDING;
    case GL_PIXEL_UNPACK_BUFFER:
      return GL_PIXEL_UNPACK_BUFFER_BINDING;
    case GL_PIXEL_PACK_BUFFER:
      return GL_PIXEL_PACK_BUFFER_BINDING;
  };
  throw std::runtime_error("unsupported targetType");
}

}  // namespace

void glCheckBound(GLuint targetType, GLuint id) {
  GLint current;
  glGetIntegerv(getBindParameter(targetType), &current);
  if (GLuint(current) != id) throw std::runtime_error("Trying to operate on unbound GlObject");
}

void checkShaderError(GLuint shaderId, const char* pSource) {
  GLint success;
  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
  if (success) return;
  GLchar InfoLog[1024];
  glGetShaderInfoLog(shaderId, sizeof(InfoLog), NULL, InfoLog);
  char error[1024];
  snprintf(error, sizeof(error), "OpenGL : error compiling shader type :\n%s\nsource :'%s'", InfoLog, pSource);
  throw std::runtime_error(error);
}

void checkProgramError(unsigned int programId) {
  GLint success;
  glGetProgramiv(programId, GL_LINK_STATUS, &success);
  if (success) return;
  GLchar InfoLog[1024];
  glGetProgramInfoLog(programId, sizeof(InfoLog), NULL, InfoLog);
  char error[1024];
  snprintf(error, sizeof(error), "OpenGL : linking shader program :\n%s\n", InfoLog);
  throw std::runtime_error(error);
}

GLenum getPixelFormat(GLint internalFormat) {
  switch (internalFormat) {
    case GL_R8:
    case GL_R32F:
      return GL_RED;
    case GL_RGB8:
    case GL_RGB16F:
    case GL_RGB32F:
    case GL_RGB16:
      return GL_RGB;
    case GL_RGBA8:
    case GL_RGBA16:
    case GL_RGBA16F:
    case GL_RGBA32F:
      return GL_RGBA;
    //    case GL_RGB16UI:
    //      return GL_RGB_INTEGER;
    case GL_RGB10_A2UI:
      return GL_RGBA_INTEGER;
    default:
      CHECK(false) << "Don't know how to convert internal image format " << getInternalFormatString(internalFormat)
                   << " to pixel format";
      return 0;
  }
}

bool isInternalOptimizedFormatRedBlueSwapped(int internalFormat) {
  switch (internalFormat) {
    case GL_R8:
    case GL_R32F:
    case GL_RGB8:
    case GL_RGBA8:
    case GL_RGB10_A2UI:
    case GL_RGB16:
    case GL_RGB16F:
    case GL_RGB32F:
    case GL_RGBA16F:
    case GL_RGBA16:
    case GL_RGBA32F:
      return false;
    default:
      return true;
  }
}

GLint getAdaptedInternalFormat(GLint internalFormat) {
  return internalFormat == GL_RGB10_A2UI ? GL_RGBA8UI : internalFormat;
}

GLenum getPixelType(GLint internalFormat) {
  switch (internalFormat) {
    case GL_R8:
    case GL_RGB8:
      return GL_UNSIGNED_BYTE;
    case GL_RGB16:
    case GL_RGBA16:
      //    case GL_RGB16UI:
      return GL_UNSIGNED_SHORT;
    case GL_RGB10_A2UI:
    case GL_RGBA8:
      return GL_UNSIGNED_INT_8_8_8_8_REV;
    case GL_RGBA16F:
    case GL_RGB16F:
      return GL_HALF_FLOAT;
    case GL_R32F:
    case GL_RGB32F:
    case GL_RGBA32F:
      return GL_FLOAT;
    default:
      CHECK(false) << "Don't know how to convert internal image format " << getInternalFormatString(internalFormat)
                   << " to pixel type";
      return 0;
  }
}

size_t getChannelCount(GLenum pixel_format) {
  switch (pixel_format) {
    case GL_RGBA:
    case GL_BGRA:
      return 4;
    case GL_RGB:
    case GL_BGR:
      return 3;
    case GL_RED:
      return 3;
    default:
      throw std::runtime_error("channel count not implemented");
  }
}

size_t getBytePerChannel(GLenum pixel_type) {
  switch (pixel_type) {
    case GL_UNSIGNED_INT_8_8_8_8:
    case GL_UNSIGNED_INT_8_8_8_8_REV:
    case GL_UNSIGNED_BYTE:
      return 1;
    case GL_UNSIGNED_SHORT:
    case GL_HALF_FLOAT:
      return 2;
    case GL_FLOAT:
    case GL_INT:
      return 4;
    default:
      throw std::runtime_error("byte per channel not implemented");
  }
}

size_t getBytePerPixels(GLenum pixel_format, GLenum pixel_type) {
  return getChannelCount(pixel_format) * getBytePerChannel(pixel_type);
}

std::string slurpFile(const char* pFilename) {
  std::ifstream in(pFilename);
  if (!in) throw std::ios_base::failure(std::string("unable to load file : ") + pFilename);
  std::stringstream sstr;
  sstr << in.rdbuf();
  return sstr.str();
}

std::vector<unsigned char> slurpBinaryFile(const char* pFilename) {
  std::ifstream in(pFilename, std::ios::in | std::ios::binary | std::ios::ate);
  if (!in) throw std::ios_base::failure(std::string("unable to load file : ") + pFilename);
  const size_t fileSize = in.tellg();
  std::vector<unsigned char> result;
  result.resize(fileSize);
  in.seekg(0, std::ios::beg);
  in.read(reinterpret_cast<char*>(result.data()), fileSize);
  return result;
}

std::pair<int, int> getTextureDimensions(size_t uwidth, size_t uheight, uint8_t orientation) {
  float width = uwidth;
  float height = uheight;
  switch (orientation) {
    case 0:  // normal (top to bottom, left to right)
    case 1:  // normal (top to bottom, left to right)
    case 2:  // flipped horizontally (top to bottom, right to left)
      height = -height;
      break;
    case 3:  // rotate 180◦ (bottom to top, right to left)
    case 4:  // flipped vertically (bottom to top, left to right)
      break;
    case 5:  // transposed (left to right, top to bottom)
    case 6:  // rotated 90◦ clockwise (right to left, top to bottom)
    case 7:  // transverse (right to left, bottom to top)
    case 8:  // rotated 90◦ counter-clockwise (left to right, bottom to top)
      throw std::runtime_error("unsupported orientation");
  }
  return std::make_pair(width, height);
}
