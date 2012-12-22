/*
 * TGA.cpp
 *
 *  Created on: Dec 2, 2012
 *      Author: Guillaume Chatelet
 */

#include "TGA.h"

#include <fstream>
#include <vector>

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

GLint gltWriteTGA(const char *szFileName) {
	GLint iViewport[4];
	glGetIntegerv(GL_VIEWPORT, iViewport);
	const unsigned long lImageSize = iViewport[2] * 3 * iViewport[3];
	std::vector<GLbyte> image(lImageSize);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ROW_LENGTH, 0);
	glPixelStorei(GL_PACK_SKIP_ROWS, 0);
	glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
	GLenum lastBuffer;
	glGetIntegerv(GL_READ_BUFFER, reinterpret_cast<GLint *>(&lastBuffer));
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, iViewport[2], iViewport[3], GL_BGR, GL_UNSIGNED_BYTE, image.data());
	glReadBuffer(lastBuffer);
	TGAHEADER tgaHeader;
	tgaHeader.identsize = 0;
	tgaHeader.colorMapType = 0;
	tgaHeader.imageType = 2;
	tgaHeader.colorMapStart = 0;
	tgaHeader.colorMapLength = 0;
	tgaHeader.colorMapBits = 0;
	tgaHeader.xstart = 0;
	tgaHeader.ystart = 0;
	tgaHeader.width = iViewport[2];
	tgaHeader.height = iViewport[3];
	tgaHeader.bits = 24;
	tgaHeader.descriptor = 0;
	std::ofstream file(szFileName, std::ios_base::binary);
	file.write(reinterpret_cast<char *>(&tgaHeader), sizeof(TGAHEADER));
	file.write(reinterpret_cast<char *>(image.data()), image.size());
	file.close();
	return 1;
}

GLbyte *gltLoadTGA(const char *szFileName, GLint *iWidth, GLint *iHeight, GLint *iComponents, GLenum *eFormat) {
	FILE *pFile;			// File pointer
	TGAHEADER tgaHeader;		// TGA file header
	unsigned long lImageSize;		// Size in bytes of image
	short sDepth;			// Pixel depth;
	GLbyte *pBits = NULL;          // Pointer to bits

	// Default/Failed values
	*iWidth = 0;
	*iHeight = 0;
	*eFormat = GL_BGR_EXT;
	*iComponents = GL_RGB8;

	// Attempt to open the fil
	pFile = fopen(szFileName, "rb");
	if (pFile == NULL)
		return NULL;

	// Read in header (binary)
	fread(&tgaHeader, 18/* sizeof(TGAHEADER)*/, 1, pFile);

	// Do byte swap for big vs little endian
#ifdef __APPLE__
	LITTLE_ENDIAN_WORD(&tgaHeader.colorMapStart);
	LITTLE_ENDIAN_WORD(&tgaHeader.colorMapLength);
	LITTLE_ENDIAN_WORD(&tgaHeader.xstart);
	LITTLE_ENDIAN_WORD(&tgaHeader.ystart);
	LITTLE_ENDIAN_WORD(&tgaHeader.width);
	LITTLE_ENDIAN_WORD(&tgaHeader.height);
#endif

	// Get width, height, and depth of texture
	*iWidth = tgaHeader.width;
	*iHeight = tgaHeader.height;
	sDepth = tgaHeader.bits / 8;

	// Put some validity checks here. Very simply, I only understand
	// or care about 8, 24, or 32 bit targa's.
	if (tgaHeader.bits != 8 && tgaHeader.bits != 24 && tgaHeader.bits != 32)
		return NULL;

	// Calculate size of image buffer
	lImageSize = tgaHeader.width * tgaHeader.height * sDepth;

	// Allocate memory and check for success
	pBits = (GLbyte*) malloc(lImageSize * sizeof(GLbyte));
	if (pBits == NULL)
		return NULL;

	// Read in the bits
	// Check for read error. This should catch RLE or other
	// weird formats that I don't want to recognize
	if (fread(pBits, lImageSize, 1, pFile) != 1) {
		free(pBits);
		return NULL;
	}

	// Set OpenGL format expected
	switch (sDepth) {
	case 3:     // Most likely case
		*eFormat = GL_BGR_EXT;
		*iComponents = GL_RGB8;
		break;
	case 4:
		*eFormat = GL_BGRA_EXT;
		*iComponents = GL_RGBA8;
		break;
	case 1:
		*eFormat = GL_LUMINANCE;
		*iComponents = GL_LUMINANCE8;
		break;
	};

	// Done with File
	fclose(pFile);

	// Return pointer to image data
	return pBits;
}

bool LoadTGATextureRect(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode) {
	GLbyte *pBits;
	int nWidth, nHeight, nComponents;
	GLenum eFormat;

	// Read the texture bits
	pBits = gltLoadTGA(szFileName, &nWidth, &nHeight, &nComponents, &eFormat);
	if (pBits == NULL)
		return false;

	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, wrapMode);

	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, magFilter);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, nComponents, nWidth, nHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBits);

	free(pBits);

	return true;
}
