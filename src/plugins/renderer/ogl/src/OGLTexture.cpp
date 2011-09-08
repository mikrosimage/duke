/*
 * OGLTexture.cpp
 *
 *  Created on: 02 juin 2010
 *      Author: Nicolas Rondaud
 */

#include "OGLTexture.h"
#include "OGLRenderer.h"
#include "OGLEnum.h"
#include <renderer/common/Setup.h>
#ifndef __APPLE__
#include <GL/gl.h>
#else
#include <OpenGL/gl.h>
#endif
#include <stdexcept>
#include <cassert>

OGLTexture::OGLTexture(const ImageDescription& description, unsigned long usageFlag, const OGLRenderer& renderer) :
    ITextureBase(description, renderer.getCompliantFormat(description.format), usageFlag), m_Texture(0), m_Pbo(renderer.getPBO()),
                    m_bUsePBO(renderer.hasCapability(CAP_PIXEL_BUFFER_OBJECT)) {

    glGenTextures(1, &m_Texture);
    TPixelFormat format = getFormat();

    if (getDepth() <= 1) {
        glBindTexture(GL_TEXTURE_2D, m_Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, OGLEnum::GetInternalFormat(format), getWidth(), getHeight(), 0, OGLEnum::GetFormat(format), OGLEnum::GetType(format), NULL);
    } else {
        glBindTexture(GL_TEXTURE_3D, m_Texture);
        glTexImage3D(GL_TEXTURE_3D, 0, OGLEnum::GetInternalFormat(format), getWidth(), getHeight(), getDepth(), 0, OGLEnum::GetFormat(format), OGLEnum::GetType(format), NULL);
    }
}

OGLTexture::~OGLTexture() {
    if (m_Texture)
        glDeleteTextures(1, &m_Texture);
}

void OGLTexture::update(const ImageDescription& description, const unsigned char* pData) {

    //  TPixelFormat format = getFormat();
    //  if (format != description.format)
    //      throw runtime_error("cannot update texture, format doesn't match");

    if (m_bUsePBO) {
        glBindTexture(GL_TEXTURE_2D, m_Texture);
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, m_Pbo);
        glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, description.width * description.height * 4, 0, GL_DYNAMIC_DRAW);
        void* ptr = glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY);
        if (ptr) {
            memcpy(ptr, pData, description.width * description.height * 4);
            glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB); // release the mapped buffer
        }
        if (description.depth <= 1) {
            glBindTexture(GL_TEXTURE_2D, m_Texture);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, description.width, description.height, OGLEnum::GetFormat(description.format), OGLEnum::GetType(description.format), 0);
        } else {
            glBindTexture(GL_TEXTURE_3D, m_Texture);
            glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, description.width, description.height, description.depth, OGLEnum::GetFormat(description.format),
                            OGLEnum::GetType(description.format), 0);
        }
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
    } else {
        if (description.depth <= 1) {
            glBindTexture(GL_TEXTURE_2D, m_Texture);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, description.width, description.height, OGLEnum::GetFormat(description.format), OGLEnum::GetType(description.format), pData);
        } else {
            glBindTexture(GL_TEXTURE_3D, m_Texture);
            glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, description.width, description.height, description.depth, OGLEnum::GetFormat(description.format),
                            OGLEnum::GetType(description.format), pData);
        }
    }

    /*
     #ifdef DEBUG
     const GLenum errCode = glGetError();
     if (errCode != GL_NO_ERROR) {
     std::cerr << "OGL Error: ";
     switch(errCode) {
     case GL_INVALID_OPERATION:
     std::cerr << "GL_INVALID_OPERATION" << std::endl;
     break;
     case GL_INVALID_VALUE:
     std::cerr << "GL_INVALID_VALUE" << std::endl;
     break;
     case GL_INVALID_ENUM:
     std::cerr << "GL_INVALID_ENUM" << std::endl;
     break;
     default:
     break;
     }
     }
     #endif
     */
}

