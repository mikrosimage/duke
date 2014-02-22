#ifdef DUKE_TGA

#include <duke/imageio/DukeIO.hpp>
#include <duke/attributes/AttributeKeys.hpp>

#include <duke/gl/GL.hpp>
#include <duke/base/ByteSwap.hpp>

#include <cstdio>

// Define targa header. This is only used locally.
#pragma pack(1)
typedef struct {
	GLbyte identsize;              // Size of ID field that follows header (0)
	GLbyte colorMapType;           // 0 = None, 1 = paletted
	GLbyte imageType;              // 0 = none, 1 = indexed, 2 = rgb, 3 = grey, +8=rle
	unsigned short colorMapStart;  // First colour map entry
	unsigned short colorMapLength; // Number of colors
	unsigned char colorMapBits;    // bits per palette entry
	unsigned short xstart;         // image x origin
	unsigned short ystart;         // image y origin
	unsigned short width;          // width in pixels
	unsigned short height;         // height in pixels
	GLbyte bits;                   // bits per pixel (8 16, 24, 32)
	GLbyte descriptor;             // image descriptor
} TGAHEADER;
#pragma pack(8)

namespace duke {

class TGAImageReader: public IImageReader {
	FILE *m_pFile;
	TGAHEADER m_Header;
public:
	TGAImageReader(const Attributes& options, const IIODescriptor *pDesc, const char*filename) :
			IImageReader(options, pDesc), m_pFile(fopen(filename, "rb")) {
		if (!m_pFile) {
			m_Error = "Unable to open";
			return;
		}
		if (fread(&m_Header, sizeof(TGAHEADER), 1, m_pFile) != 1) {
			m_Error = "Unable to read header";
			return;
		}
	}

	virtual bool doSetup(PackedFrameDescription& description, Attributes& attributes) override {
		switch (m_Header.bits) {
		case 24:     // Most likely case
		    description.glPackFormat = GL_RGB8;
			break;
		case 32:
		    description.glPackFormat = GL_RGBA8;
			break;
		case 8:
		    description.glPackFormat = GL_R8;
			break;
		default:
			m_Error = "Unsupported bit depth";
			return false;
		};
		description.swapRedAndBlue = true;
		description.width = m_Header.width;
		description.height = m_Header.height;
		description.dataSize = m_Header.width * m_Header.height * (m_Header.bits / 8);
		attributes.set<attribute::DpxImageOrientation>(4);
		return true;
	}

	~TGAImageReader() {
		if (m_pFile)
			fclose(m_pFile);
	}

	virtual void readImageDataTo(void* pData) {
		if (hasError() || !pData)
			return;
        const size_t dataSize = m_Header.width * m_Header.height * (m_Header.bits / 8);
        if (fread(pData, dataSize, 1, m_pFile) != 1)
            m_Error = "Unable to read from file";
	}
};

class TGADescriptor: public IIODescriptor {
	virtual ~TGADescriptor() {
	}
	virtual const std::vector<std::string>& getSupportedExtensions() const override {
		static std::vector<std::string> extensions = { "tga" };
		return extensions;
	}
	virtual bool supports(Capability capability) const override {
	    return capability == Capability::READER_FILE_SEQUENCE;
	}
	virtual const char* getName() const override {
		return "Targa";
	}
	virtual IImageReader* getReaderFromFile(const Attributes& options, const char *filename) const override {
		return new TGAImageReader(options, this, filename);
	}
};

namespace {
bool registrar = IODescriptors::instance().registerDescriptor(new TGADescriptor());
}  // namespace

}  // namespace duke

#endif // DUKE_TGA
