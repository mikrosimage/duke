#include "OGLTexture.h"
#include "OGLRenderer.h"
#include "OGLEnum.h"
#include <dukerenderer/plugin/utils/PixelUtils.h>
#include <dukerenderer/Setup.h>
#ifndef __APPLE__
#include <GL/gl.h>
#else
#include <OpenGL/gl.h>
#endif
#include <stdexcept>
#include <cassert>

OGLTexture::OGLTexture(const ImageDescription& description, unsigned long usageFlag, const OGLRenderer& renderer) :
    ITextureBase(description, renderer.getCompliantFormat(description.format), usageFlag), m_Texture(0), m_Renderer(renderer) {

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

    OGLRenderer &r = const_cast<OGLRenderer&> (m_Renderer);
    if (r.hasCapability(CAP_PIXEL_BUFFER_OBJECT)) {

        const int imageLineSize = description.width * GetBytesPerPixel(getFormat());
        const int dataSize = imageLineSize * description.height;

        glBindTexture(GL_TEXTURE_2D, m_Texture);
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, r.getPBO());
        glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, dataSize, pData, GL_STREAM_DRAW_ARB);
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

#ifdef DEBUG
    const GLenum errCode = glGetError();
    if (errCode != GL_NO_ERROR) {
        std::cerr << "OGL Error: ";
        switch (errCode) {
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

}

