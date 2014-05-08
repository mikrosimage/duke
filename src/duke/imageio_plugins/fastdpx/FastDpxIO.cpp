#ifdef DUKE_FAST_DPX

#include <duke/attributes/AttributeKeys.hpp>  // for DpxImageOrientation
#include <duke/attributes/Attributes.hpp>     // for Attributes
#include <duke/base/ByteSwap.hpp>             // for bswap_32
#include <duke/gl/GL.hpp>
#include <duke/image/FrameDescription.hpp>
#include <duke/imageio/DukeIO.hpp>  // for IIODescriptor::Capability, etc

#include <stddef.h>  // for size_t
#include <stdint.h>  // for int32_t
#include <string>    // for string
#include <vector>    // for vector

#define DPX_MAGIC 0x53445058
#define DPX_MAGIC_SWAP 0x58504453

typedef struct file_information {
  unsigned int magic_num;      /* magic number 0x53445058 (SDPX) big endian or 0x58504453 (XPDS) little endian */
  unsigned int offset;         /* offset to image data in bytes */
  char vers[8];                /* which header format version is being used (v1.0)*/
  unsigned int file_size;      /* file size in bytes */
  unsigned int ditto_key;      /* read time short cut - 0 = same, 1 = new */
  unsigned int gen_hdr_size;   /* generic header length in bytes */
  unsigned int ind_hdr_size;   /* industry header length in bytes */
  unsigned int user_data_size; /* user-defined data length in bytes */
  char file_name[100];         /* image file name */
  char create_time[24];        /* file creation date "yyyy:mm:dd:hh:mm:ss:LTZ" */
  char creator[100];           /* file creator's name */
  char project[200];           /* project name */
  char copyright[200];         /* right to use or copyright info */
  unsigned int key;            /* encryption ( FFFFFFFF = unencrypted ) */
  char Reserved[104];          /* reserved field TBD (need to pad) */
} FileInformation;

typedef struct _image_information {
  unsigned short orientation;       /* image orientation */
  unsigned short element_number;    /* number of image elements */
  unsigned int pixels_per_line;     /* or x value */
  unsigned int lines_per_image_ele; /* or y value, per element */
  struct _image_element {
    unsigned int data_sign; /* data sign (0 = unsigned, 1 = signed ) */
    /* "Core set images are unsigned" */
    unsigned int ref_low_data;     /* reference low data code value */
    float ref_low_quantity;        /* reference low quantity represented */
    unsigned int ref_high_data;    /* reference high data code value */
    float ref_high_quantity;       /* reference high quantity represented */
    unsigned char descriptor;      /* descriptor for image element */
    unsigned char transfer;        /* transfer characteristics for element */
    unsigned char colorimetric;    /* colormetric specification for element */
    unsigned char bit_size;        /* bit size for element */
    unsigned short packing;        /* packing for element */
    unsigned short encoding;       /* encoding for element */
    unsigned int data_offset;      /* offset to data of element */
    unsigned int eol_padding;      /* end of line padding used in element */
    unsigned int eo_image_padding; /* end of image padding used in element */
    char description[32];          /* description of element */
  } image_element[8];              /* NOTE THERE ARE EIGHT OF THESE */

  unsigned char reserved[52]; /* reserved for future use (padding) */
} Image_Information;

namespace duke {

class FastDpxImageReader : public IImageReader {
  const void* m_pData;
  const FileInformation* pInformation;
  const char* pArithmeticPointer;
  const Image_Information* pImageInformation;
  const unsigned int magic;
  const bool bigEndian;

  template <typename T>
  inline T swap(T value) const {
    return ::swap<T>(value, bigEndian);
  }

 public:
  FastDpxImageReader(const attribute::Attributes& options, const void* pData, const size_t dataSize)
      : IImageReader(options),
        m_pData(nullptr),
        pInformation(reinterpret_cast<const FileInformation*>(pData)),
        pArithmeticPointer(reinterpret_cast<const char*>(pData)),
        pImageInformation(reinterpret_cast<const Image_Information*>(pArithmeticPointer + sizeof(FileInformation))),
        magic(pInformation->magic_num),
        bigEndian(magic == DPX_MAGIC_SWAP) {
    if (magic != DPX_MAGIC_SWAP && magic != DPX_MAGIC) {
      m_Error = "invalid magic : not a dpx file";
      return;
    }
    const auto& image = pImageInformation->image_element[0];
    const auto bitSize = swap(image.bit_size);
    const auto componentsFilled = swap(image.packing) == 1;
    if (bitSize != 10 || !componentsFilled) {
      m_Error = "Can't use fast dpx";
      return;
    }
  }

  virtual bool doSetup(FrameDescription& description, attribute::Attributes& attributes) override {
    m_pData = nullptr;
    description.height = swap(pImageInformation->lines_per_image_ele);
    description.width = swap(pImageInformation->pixels_per_line);
    m_pData = pArithmeticPointer + swap(pInformation->offset);
    description.swapEndianness = bigEndian;
    description.glFormat = GL_RGB10_A2UI;
    description.dataSize = description.height * description.width * sizeof(int32_t);
    attribute::set<attribute::DpxImageOrientation>(attributes, pImageInformation->orientation);
    return true;
  }

  virtual const void* getMappedImageData() const override { return m_pData; }
};

class FastDpxDescriptor : public IIODescriptor {
  virtual bool supports(Capability capability) const override {
    return capability == Capability::READER_READ_FROM_MEMORY || capability == Capability::READER_FILE_SEQUENCE;
  }
  virtual const std::vector<std::string>& getSupportedExtensions() const override {
    static std::vector<std::string> extensions = {"dpx"};
    return extensions;
  }
  virtual const char* getName() const override { return "FastDpx"; }
  virtual IImageReader* createMemoryReader(const attribute::Attributes& options, const void* pData,
                                           const size_t dataSize) const override {
    return new FastDpxImageReader(options, pData, dataSize);
  }
};

namespace {
bool registrar = IODescriptors::instance().registerDescriptor(new FastDpxDescriptor());
}  // namespace

}  // namespace duke

#endif  // DUKE_FAST_DPX
