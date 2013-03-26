#pragma once

#include <string>
#include <vector>

void setTextureDimensions(unsigned int dimensionUniformParameter, size_t uwidth, size_t uheight, int orientation = 1);

int getAdaptedInternalFormat(int internalFormat);
unsigned int getPixelFormat(int internalFormat);
unsigned int getPixelType(int internalFormat);
bool isInternalOptimizedFormatRedBlueSwapped(int internalFormat);

const char* getInternalFormatString(int internalFormat);
const char* getPixelFormatString(unsigned int pixelFormat);
const char* getPixelTypeString(unsigned int pixelType);

size_t getChannelCount(unsigned int pixel_format);
size_t getBytePerChannel(unsigned int pixel_type);
size_t getBytePerPixels(unsigned int pixel_format, unsigned int pixel_type);

void glCheckError();
void glCheckBound(unsigned int targetType, unsigned int id);
void checkShaderError(unsigned int shaderId, const char* source);
void checkProgramError(unsigned int programId);

std::string slurpFile(const char* pFilename);
std::vector<unsigned char> slurpBinaryFile(const char* pFilename);
