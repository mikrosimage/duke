/*
 * TgaIO.cpp
 *
 *  Created on: Dec 15, 2012
 *      Author: Guillaume Chatelet
 */

#include <duke/imageio/DukeIO.h>

#include <duke/gl/GL.h>

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

class TGAImageReader: public IImageReader {
	FILE *m_pFile;
	TGAHEADER m_Header;
public:
	TGAImageReader(const IIODescriptor *pDesc, const char*filename) :
			IImageReader(pDesc), m_pFile(fopen(filename, "rb")) {
		if (!m_pFile) {
			m_Error = "Unable to open";
			return;
		}
		if (fread(&m_Header, sizeof(TGAHEADER), 1, m_pFile) != 1) {
			m_Error = "Unable to read header";
			return;
		}
		// Do byte swap for big vs little endian
#ifdef __APPLE__
		LITTLE_ENDIAN_WORD(&m_Header.colorMapStart);
		LITTLE_ENDIAN_WORD(&m_Header.colorMapLength);
		LITTLE_ENDIAN_WORD(&m_Header.xstart);
		LITTLE_ENDIAN_WORD(&m_Header.ystart);
		LITTLE_ENDIAN_WORD(&m_Header.width);
		LITTLE_ENDIAN_WORD(&m_Header.height);
#endif

		switch (m_Header.bits) {
		case 24:     // Most likely case
			m_Description.glFormat = GL_BGR;
			break;
		case 32:
			m_Description.glFormat = GL_BGRA;
			break;
		case 8:
			m_Description.glFormat = GL_RED;
			break;
		default:
			m_Error = "Unsupported bit depth";
			return;
		};
		m_Description.glType = GL_UNSIGNED_BYTE;
		m_Description.width = m_Header.width;
		m_Description.height = m_Header.height;
		m_Description.depth = 1;
		m_Description.dataSize = m_Header.width * m_Header.height * (m_Header.bits / 8);
		m_Attributes.push_back(Attribute("Orientation", (int) 4));
	}
	~TGAImageReader() {
		if (m_pFile)
			fclose(m_pFile);
	}
	virtual void readImageDataTo(void* pData) {
		if (hasError() || !pData)
			return;
		if (fread(pData, m_Description.dataSize, 1, m_pFile) != 1)
			m_Error = "Unable to read from file";
	}
};

class TGADescriptor: public IIODescriptor {
	virtual ~TGADescriptor() {
	}
	virtual const std::vector<std::string>& getSupportedExtensions() const {
		static std::vector<std::string> extensions = { "tga" };
		return extensions;
	}
	virtual bool supports(Capability capability) const {
		return false;
	}
	virtual const char* getName() const {
		return "Targa";
	}
	virtual IImageReader* getReaderFromFile(const char *filename) const {
		return new TGAImageReader(this, filename);
	}
};

namespace {
bool registrar = IODescriptors::instance().registerDescriptor(new TGADescriptor());
}  // namespace
