/*
 * TGA.h
 *
 *  Created on: Dec 2, 2012
 *      Author: Guillaume Chatelet
 */

#ifndef TGA_H_
#define TGA_H_

#include <GL/gl.h>
GLint gltWriteTGA(const char *szFileName);
GLbyte *gltLoadTGA(const char *szFileName, GLint *iWidth, GLint *iHeight, GLint *iComponents, GLenum *eFormat);
bool LoadTGATextureRect(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode);
#endif /* TGA_H_ */
