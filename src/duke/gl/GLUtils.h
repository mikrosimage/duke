/*
 * GLUtils.h
 *
 *  Created on: Nov 28, 2012
 *      Author: Guillaume Chatelet
 */

#ifndef GLUTILS_H_
#define GLUTILS_H_

#include <string>

void setTextureDimensions(unsigned int dimensionUniformParameter, size_t uwidth, size_t uheight, int orientation = 1);

int getAdaptedInternalFormat(int internalFormat);
unsigned int getPixelFormat(int internalFormat);
unsigned int getPixelType(int internalFormat);
bool isInternalOptimizedFormatRedBlueSwapped(int internalFormat);

const char* getInternalFormatString(int internalFormat);
const char* getPixelFormatString(unsigned int pixelFormat);
const char* getPixelTypeString(unsigned int pixelType);

void glCheckError();
void glCheckBound(unsigned int targetType, unsigned int id);
void checkShaderError(unsigned int shaderId, const char* source);
void checkProgramError(unsigned int programId);

std::string slurpFile(const char* pFilename);

#endif /* GLUTILS_H_ */
