#ifdef DUKE_OIIO

#include "duke/attributes/Attribute.hpp"
#include "duke/io/IO.hpp"
#include "duke/image/ImageUtils.hpp"
#include "duke/gl/GL.hpp"

#include <OpenImageIO/imageio.h>

#include <algorithm>
#include <iterator>
#include <set>

using namespace std;
OIIO_NAMESPACE_USING;

namespace duke {

namespace {

Channels::FormatType getFormatType(const TypeDesc& description) {
  switch (description.basetype) {
    case TypeDesc::UINT8:
    case TypeDesc::UINT16:
    case TypeDesc::UINT32:
      return Channels::FormatType::UNSIGNED_INTEGRAL;
    case TypeDesc::INT8:
    case TypeDesc::INT16:
    case TypeDesc::INT32:
      return Channels::FormatType::SIGNED_INTEGRAL;
    case TypeDesc::HALF:
    case TypeDesc::FLOAT:
      return Channels::FormatType::FLOATING_POINT;
  }
  CHECK(false) << "Unsupported basetype " << description.basetype;
  return Channels::FormatType::UNKNOWN;
}

uint8_t getBits(const TypeDesc& description) {
  switch (description.basetype) {
    case TypeDesc::UINT8:
    case TypeDesc::INT8:
      return 8;
    case TypeDesc::INT16:
    case TypeDesc::UINT16:
    case TypeDesc::HALF:
      return 16;
    case TypeDesc::UINT32:
    case TypeDesc::INT32:
    case TypeDesc::FLOAT:
      return 32;
  }
  CHECK(false) << "Unsupported basetype " << description.basetype;
  return 0;
}

Channel::Semantic getSemantic(const string& name, bool isAlpha, bool isDepth) {
  if (isAlpha || name == "A") return Channel::Semantic::ALPHA;
  if (isDepth || name == "Z") return Channel::Semantic::DEPTH;
  if (name == "R") return Channel::Semantic::RED;
  if (name == "G") return Channel::Semantic::GREEN;
  if (name == "B") return Channel::Semantic::BLUE;
  return Channel::Semantic::UNKNOWN;
}

Channels getChannels(const ImageSpec& imageSpec) {
  Channels channels;
  set<Channels::FormatType> formatTypes;
  CHECK(imageSpec.channelformats.empty()) << "Per channel format unsupported";
  for (size_t i = 0; i < imageSpec.channelnames.size(); ++i) {
    const auto& format = imageSpec.format;
    const auto& name = imageSpec.channelnames.at(i);
    const bool isAlpha = i == static_cast<size_t>(imageSpec.alpha_channel);
    const bool isDepth = i == static_cast<size_t>(imageSpec.z_channel);
    channels.emplace_back(getSemantic(name, isAlpha, isDepth), getBits(format), name);
    formatTypes.insert(getFormatType(format));
  }
  CHECK(formatTypes.size() == 1) << "All channels must conform to the same type";
  channels.type = *formatTypes.begin();
  return channels;
}

ImageDescription getImageDescription(const ImageSpec& imageSpec) {
  ImageDescription description;
  description.x = imageSpec.x;
  description.y = imageSpec.y;
  description.width = imageSpec.width;
  description.height = imageSpec.height;
  description.full_x = imageSpec.full_x;
  description.full_y = imageSpec.full_y;
  description.full_width = imageSpec.full_width;
  description.full_height = imageSpec.full_height;
  description.tile_width = imageSpec.tile_width;
  description.tile_height = imageSpec.tile_height;
  description.channels = getChannels(imageSpec);
  return description;
}

template <typename T>
void insert(attribute::Attributes& attributes, const char* const key, const void* const ptr, int aggregate) {
  CHECK(aggregate > 0);
  const auto* pBegin = reinterpret_cast<const T*>(ptr);
  if (aggregate == 1)
    attribute::set(attributes, key, *pBegin);
  else
    attribute::set(attributes, key, Slice<const T>(pBegin, pBegin + aggregate));
}

}  // namespace

class OpenImageIOReader : public IImageReader {
  unique_ptr<ImageInput> m_pImageInput;
  ImageSpec m_Spec;

 public:
  OpenImageIOReader(const char* filename) : m_pImageInput(ImageInput::create(filename)) {
    if (!m_pImageInput) {
      m_Error = OpenImageIO::geterror();
      return;
    }
    if (!m_pImageInput->open(filename, m_Spec)) {
      m_Error = OpenImageIO::geterror();
      return;
    }
    if (m_Spec.depth > 1) {
      m_Error = "can't read volume images";
      return;
    }
    static const vector<string> A = {"A"};
    static const vector<string> RGB = {"R", "G", "B"};
    static const vector<string> RGBA = {"R", "G", "B", "A"};
    if (m_Spec.channelnames != RGB && m_Spec.channelnames != RGBA && m_Spec.channelnames != A) {
      m_Error = "Can only handle RGB, RGBA and A images for now, was '";
      for (const auto& string : m_Spec.channelnames) m_Error += string;
      m_Error += "'";
      return;
    }

    m_Description.frames = 1;
    // images
    m_Description.subimages.push_back(getImageDescription(m_Spec));
    // metadata
    auto& metadata = m_Description.metadata;
    for (const ParamValue& paramvalue : m_Spec.extra_attribs) {
      // skipping none scalar type for now
      if (paramvalue.nvalues() != 1) {
        continue;
      }
      // For a string only the pointer is stored in the oiio attribute
      // we need to copy the whole data to prevent reading dangling pointers.
      const char* key = paramvalue.name().c_str();
      const void* data = paramvalue.data();
      const auto aggregate = paramvalue.type().aggregate;
      switch (paramvalue.type().basetype) {
        case TypeDesc::STRING:
          attribute::set(metadata, key, *reinterpret_cast<const char* const*>(data));
          break;
        case TypeDesc::INT8:
          insert<int8_t>(metadata, key, data, aggregate);
          break;
        case TypeDesc::INT16:
          insert<int16_t>(metadata, key, data, aggregate);
          break;
        case TypeDesc::INT32:
          insert<int32_t>(metadata, key, data, aggregate);
          break;
        case TypeDesc::INT64:
          insert<int64_t>(metadata, key, data, aggregate);
          break;
        case TypeDesc::UINT8:
          insert<uint8_t>(metadata, key, data, aggregate);
          break;
        case TypeDesc::UINT16:
          insert<uint16_t>(metadata, key, data, aggregate);
          break;
        case TypeDesc::UINT32:
          insert<uint32_t>(metadata, key, data, aggregate);
          break;
        case TypeDesc::UINT64:
          insert<uint64_t>(metadata, key, data, aggregate);
          break;
        case TypeDesc::FLOAT:
          insert<float>(metadata, key, data, aggregate);
          break;
        case TypeDesc::DOUBLE:
          insert<double>(metadata, key, data, aggregate);
          break;
        default:
          CHECK(false) << "Unhandled type";
      }
    }
  }

  ~OpenImageIOReader() {
    if (m_pImageInput) m_pImageInput->close();
  }

  bool read(const ReadOptions& options, const Allocator& allocator, FrameData& frame) override {
    if (options.frame != 0) return error("plugin does not support multiple frames");
    if (options.subimage != 0) return error("plugin does not support subimage yet");
    auto description = m_Description.subimages.at(0);
    auto data = frame.setDescriptionAndAllocate(description, allocator);
    if (!m_pImageInput->read_image(m_Spec.format, data.begin())) return error(OpenImageIO::geterror());
    return true;
  }
};

class OpenImageIODescriptor : public IIODescriptor {
  vector<string> m_Extensions;

 public:
  OpenImageIODescriptor() {
    string extensions;
    getattribute("extension_list", extensions);
    bool append = false;
    string current;
    for (const char c : extensions) {
      if (c == ';') append = false;
      if (c == ',' || c == ';') {
        m_Extensions.push_back(current);
        current.clear();
      } else if (append)
        current.push_back(c);
      if (c == ':') append = true;
    }
    m_Extensions.push_back(current);
  }
  virtual bool supports(Capability capability) const override {
    return capability == Capability::READER_GENERAL_PURPOSE || capability == Capability::READER_FILE_SEQUENCE;
  }
  virtual const vector<string>& getSupportedExtensions() const override { return m_Extensions; }
  virtual const char* getName() const override { return "OpenImageIO"; }
  virtual IImageReader* createFileReader(const char* filename) const override {
    return new OpenImageIOReader(filename);
  }
};

namespace {
bool registrar = IODescriptors::instance().registerDescriptor(new OpenImageIODescriptor());
}  // namespace

}  // namespace duke

#endif  // DUKE_OIIO
