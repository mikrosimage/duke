#ifdef DUKE_TGA

#include <duke/io/IO.hpp>
#include <duke/attributes/AttributeKeys.hpp>

#include <duke/gl/GL.hpp>
#include <duke/base/ByteSwap.hpp>

#include <cstdio>

using namespace attribute;

// Define targa header. This is only used locally.
#pragma pack(1)
typedef struct {
  GLbyte identsize;               // Size of ID field that follows header (0)
  GLbyte colorMapType;            // 0 = None, 1 = paletted
  GLbyte imageType;               // 0 = none, 1 = indexed, 2 = rgb, 3 = grey, +8=rle
  unsigned short colorMapStart;   // First colour map entry
  unsigned short colorMapLength;  // Number of colors
  unsigned char colorMapBits;     // bits per palette entry
  unsigned short xstart;          // image x origin
  unsigned short ystart;          // image y origin
  unsigned short width;           // width in pixels
  unsigned short height;          // height in pixels
  GLbyte bits;                    // bits per pixel (8 16, 24, 32)
  GLbyte descriptor;              // image descriptor
} TGAHEADER;
#pragma pack(8)

namespace duke {

class TGAImageReader : public IImageReader {
  FILE* m_pFile;
  TGAHEADER m_Header;

 public:
  TGAImageReader(const char* filename) : m_pFile(fopen(filename, "rb")) {
    if (!m_pFile) {
      m_Error = "Unable to open";
      return;
    }
    if (fread(&m_Header, sizeof(TGAHEADER), 1, m_pFile) != 1) {
      m_Error = "Unable to read header";
      return;
    }
    ImageDescription description;
    switch (m_Header.bits) {
      case 24:  // Most likely case
        description.channels = getChannels(GL_RGB8);
        break;
      case 32:
        description.channels = getChannels(GL_RGBA8);
        break;
      case 8:
        description.channels = getChannels(GL_R8);
        break;
      default:
        m_Error = "Unsupported bit depth";
        return;
    };
    description.width = m_Header.width;
    description.height = m_Header.height;
    m_Description.frames = 1;
    m_Description.subimages.push_back(std::move(description));
  }

  ~TGAImageReader() {
    if (m_pFile) fclose(m_pFile);
  }

  bool read(const ReadOptions& options, const Allocator& allocator, FrameData& frame) override {
    using namespace attribute;
    auto description = m_Description.subimages.at(0);
    auto& attributes = description.extra_attributes;
    set<DpxImageOrientation>(attributes, 4);
    set<ImageSwapRedAndBlue>(attributes, true);
    auto data = frame.setDescriptionAndAllocate(description, allocator);
    if (fread(data.begin(), data.size(), 1, m_pFile) != 1) m_Error = "Unable to read from file";
    return true;
  }
};

class TGADescriptor : public IIODescriptor {
  virtual ~TGADescriptor() {}
  virtual const std::vector<std::string>& getSupportedExtensions() const override {
    static std::vector<std::string> extensions = {"tga"};
    return extensions;
  }
  virtual bool supports(Capability capability) const override { return capability == Capability::READER_FILE_SEQUENCE; }
  virtual const char* getName() const override { return "Targa"; }
  virtual IImageReader* createFileReader(const char* filename) const override { return new TGAImageReader(filename); }
};

namespace {
bool registrar = IODescriptors::instance().registerDescriptor(new TGADescriptor());
}  // namespace

}  // namespace duke

#endif  // DUKE_TGA
