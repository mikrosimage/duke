#ifdef DUKE_OIIO

#include <duke/attributes/Attribute.hpp>
#include <duke/imageio/DukeIO.hpp>
#include <duke/gl/GL.hpp>

#include <OpenImageIO/imageio.h>

#include <algorithm>
#include <iterator>

using namespace std;
OIIO_NAMESPACE_USING;

namespace duke {

namespace {

const vector<string> A = {"A"};
const vector<string> RGB = {"R", "G", "B"};
const vector<string> RGBA = {"R", "G", "B", "A"};

GLuint getGlType(const TypeDesc& typedesc, const vector<string>& channels) {
  if (channels == A) {
    switch (typedesc.basetype) {
      case TypeDesc::UCHAR:
      case TypeDesc::CHAR:
        return GL_R8;
      case TypeDesc::USHORT:
      case TypeDesc::SHORT:
        return GL_R16;
      case TypeDesc::UINT:
        return GL_R32UI;
      case TypeDesc::INT:
        return GL_R32I;
      case TypeDesc::HALF:
        return GL_R16F;
      case TypeDesc::FLOAT:
        return GL_R32F;
    }
  } else if (channels == RGB) {
    switch (typedesc.basetype) {
      case TypeDesc::UCHAR:
      case TypeDesc::CHAR:
        return GL_RGB8;
      case TypeDesc::USHORT:
      case TypeDesc::SHORT:
        return GL_RGB16;
      case TypeDesc::UINT:
        return GL_RGB32UI;
      case TypeDesc::INT:
        return GL_RGB32I;
      case TypeDesc::HALF:
        return GL_RGB16F;
      case TypeDesc::FLOAT:
        return GL_RGB32F;
    }
  } else if (channels == RGBA) {
    switch (typedesc.basetype) {
      case TypeDesc::UCHAR:
      case TypeDesc::CHAR:
        return GL_RGBA8;
      case TypeDesc::USHORT:
      case TypeDesc::SHORT:
        return GL_RGBA16;
      case TypeDesc::UINT:
        return GL_RGBA32UI;
      case TypeDesc::INT:
        return GL_RGBA32I;
      case TypeDesc::HALF:
        return GL_RGBA16F;
      case TypeDesc::FLOAT:
        return GL_RGBA32F;
    }
  }
  return 0;
}

size_t getTypeSize(const TypeDesc& typedesc) {
  switch (typedesc.basetype) {
    case TypeDesc::UCHAR:
    case TypeDesc::CHAR:
      return 1;
    case TypeDesc::USHORT:
    case TypeDesc::SHORT:
    case TypeDesc::HALF:
      return 2;
    case TypeDesc::UINT:
    case TypeDesc::INT:
    case TypeDesc::FLOAT:
      return 4;
    case TypeDesc::DOUBLE:
      return 8;
    default:
      return 0;
  }
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
  OpenImageIOReader(const attribute::Attributes& options, const char* filename)
      : IImageReader(options), m_pImageInput(ImageInput::create(filename)) {
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
    if (m_Spec.channelnames != RGB && m_Spec.channelnames != RGBA && m_Spec.channelnames != A) {
      m_Error = "Can only handle RGB, RGBA and A images for now, was '";
      for (const auto& string : m_Spec.channelnames) m_Error += string;
      m_Error += "'";
      return;
    }
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
          attribute::set(m_ReaderAttributes, key, *reinterpret_cast<const char* const*>(data));
          break;
        case TypeDesc::INT8:
          insert<int8_t>(m_ReaderAttributes, key, data, aggregate);
          break;
        case TypeDesc::INT16:
          insert<int16_t>(m_ReaderAttributes, key, data, aggregate);
          break;
        case TypeDesc::INT32:
          insert<int32_t>(m_ReaderAttributes, key, data, aggregate);
          break;
        case TypeDesc::INT64:
          insert<int64_t>(m_ReaderAttributes, key, data, aggregate);
          break;
        case TypeDesc::UINT8:
          insert<uint8_t>(m_ReaderAttributes, key, data, aggregate);
          break;
        case TypeDesc::UINT16:
          insert<uint16_t>(m_ReaderAttributes, key, data, aggregate);
          break;
        case TypeDesc::UINT32:
          insert<uint32_t>(m_ReaderAttributes, key, data, aggregate);
          break;
        case TypeDesc::UINT64:
          insert<uint64_t>(m_ReaderAttributes, key, data, aggregate);
          break;
        case TypeDesc::FLOAT:
          insert<float>(m_ReaderAttributes, key, data, aggregate);
          break;
        case TypeDesc::DOUBLE:
          insert<double>(m_ReaderAttributes, key, data, aggregate);
          break;
        default:
          CHECK(false) << "Unhandled type";
      }
    }
  }

  ~OpenImageIOReader() {
    if (m_pImageInput) m_pImageInput->close();
  }

  virtual bool doSetup(FrameDescription& description, attribute::Attributes& attributes) override {
    description.width = m_Spec.width;
    description.height = m_Spec.height;
    description.glFormat = getGlType(m_Spec.format, m_Spec.channelnames);
    description.dataSize = m_Spec.width * m_Spec.height * m_Spec.nchannels * getTypeSize(m_Spec.format);
    return true;
  }

  virtual void readImageDataTo(void* pData) {
    if (!m_pImageInput) return;
    if (!m_pImageInput->read_image(m_Spec.format, pData)) {
      m_Error = OpenImageIO::geterror();
      return;
    }
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
  virtual IImageReader* createFileReader(const attribute::Attributes& options, const char* filename) const override {
    return new OpenImageIOReader(options, filename);
  }
};

namespace {
bool registrar = IODescriptors::instance().registerDescriptor(new OpenImageIODescriptor());
}  // namespace

}  // namespace duke

#endif  // DUKE_OIIO
