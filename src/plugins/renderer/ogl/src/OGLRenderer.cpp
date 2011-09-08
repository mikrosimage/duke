/*
 * OGLRenderer.cpp
 *
 *  Created on: 02 juin 2010
 *      Author: Nicolas Rondaud
 */

#include "OGLRenderer.h"
#include "OGLBuffer.h"
#include "OGLShader.h"
#include "OGLEnum.h"
#include "OGLTexture.h"
#include <renderer/plugin/common/Mesh.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <Cg/cgGL.h>

using namespace ::google::protobuf;
using namespace ::protocol::duke;

#define BUFFER_OFFSET( n ) ( (char*)NULL + ( n ) )

int CheckGLError(const char* msg, const char* file, int line) {
    GLenum glErr;
    int retCode = 0;

    if ((glErr = glGetError()) != GL_NO_ERROR) {
        std::cerr << "GL_ERROR :" << msg << std::endl;
        switch (glErr) {
            case GL_INVALID_ENUM:
                std::cerr << "GL_INVALID_ENUM error in File " << file << " at line: " << line << std::endl;
                break;
            case GL_INVALID_VALUE:
                std::cerr << "GL_INVALID_VALUE error in File " << file << " at line: " << line << std::endl;
                break;
            case GL_INVALID_OPERATION:
                std::cerr << "GL_INVALID_OPERATION error in File " << file << " at line: " << line << std::endl;
                break;
            case GL_STACK_OVERFLOW:
                std::cerr << "GL_STACK_OVERFLOW error in File " << file << " at line: " << line << std::endl;
                break;
            case GL_STACK_UNDERFLOW:
                std::cerr << "GL_STACK_UNDERFLOW error in File " << file << " at line: " << line << std::endl;
                break;
            case GL_OUT_OF_MEMORY:
                std::cerr << "GL_OUT_OF_MEMORY error in File " << file << " at line: " << line << std::endl;
                break;
            default:
                std::cerr << "UNKNOWN ERROR error in File " << file << " at line: " << line << std::endl;
        }
    }
    return retCode;
}

OGLRenderer::OGLRenderer(const protocol::duke::Renderer& Renderer, sf::Window& window, const RendererSuite& suite) :
    IRenderer(Renderer, window, suite) {

    // Initializing OGL Extensions
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::ostringstream msg;
        msg << "OGLRenderer: " << glewGetErrorString(err) << std::endl;
        throw std::invalid_argument(msg.str());
    }
    if (Renderer.refreshrate() != 0 && Renderer.refreshrate() < 57) {
        std::ostringstream msg;
        msg << "Cannot force refresh rate to " << Renderer.refreshrate() << "Hz because it's less than 57Hz";
        throw std::invalid_argument(msg.str());
    }

    // Set the color and depth clear values
    glClearDepth(1.f);
    glClearColor(0.f, 0.f, 0.f, 1.f);

    // Enable Z-buffer read and write
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_3D);
    glEnable(GL_DEPTH_TEST);

    glShadeModel (GL_FLAT);
    glDisable (GL_CULL_FACE);
    glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

    // Setup a perspective projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, Renderer.width(), Renderer.height());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    checkCaps();

    cgGLRegisterStates(getCgContext());
    m_VSProfile = cgGLGetLatestProfile(CG_GL_VERTEX);
    m_PSProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
    cgGLSetOptimalOptions(m_VSProfile);
    cgGLSetOptimalOptions(m_PSProfile);

    if(hasCapability(CAP_PIXEL_BUFFER_OBJECT)){
        glGenBuffersARB(1, &m_Pbo);
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, m_Pbo);
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
    }

    // Create a framebuffer object
    glGenFramebuffersEXT(1, &m_Fbo);
    glGenRenderbuffersEXT(1, &m_RenderBuffer);
}

OGLRenderer::~OGLRenderer() {
    if (m_Pbo)
        glDeleteBuffersARB(1, &m_Pbo);
}

// IFactory
IBufferBase* OGLRenderer::createVB(unsigned long size, unsigned long stride, unsigned long flags) const {
    GLuint vertexBuffer = 0;

    glGenBuffersARB(1, &vertexBuffer);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertexBuffer);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, size * stride, NULL, OGLEnum::BufferFlags(flags));
    return (new OGLVertexBuffer(size, vertexBuffer));
}

IBufferBase* OGLRenderer::createIB(unsigned long size, unsigned long stride, unsigned long flags) const {
    GLuint indexBuffer = 0;

    glGenBuffersARB(1, &indexBuffer);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indexBuffer);
    glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, size * stride, NULL, OGLEnum::BufferFlags(flags));
    return new OGLIndexBuffer(size, indexBuffer);
}

IShaderBase* OGLRenderer::createShader(CGprogram program, TShaderType type) const {
    return (new OGLShader(program, type));
}

TPixelFormat OGLRenderer::getCompliantFormat(TPixelFormat format) const {
    switch (format) {
        case PXF_R10G10B10A2:
        case PXF_A2B10G10R10:
        case PXF_R8G8B8A8:
            return PXF_B8G8R8A8;
        case PXF_R8G8B8:
            return PXF_B8G8R8;
        default:
            return format;
    }
}

ITextureBase* OGLRenderer::createTexture(const ImageDescription& description, unsigned long usageFlags) const {
    if (description.format == PXF_UNDEFINED)
        return NULL;
    return (new OGLTexture(description, usageFlags, *this));
}

void OGLRenderer::checkCaps() {
    m_Capabilities[CAP_TEX_NON_POWER_2] = GLEW_ARB_texture_non_power_of_two;
    m_Capabilities[CAP_HW_MIPMAPPING] = GLEW_SGIS_generate_mipmap;
    m_Capabilities[CAP_PIXEL_BUFFER_OBJECT] = false; //GLEW_ARB_pixel_buffer_object
}

// IRenderer
void OGLRenderer::setVertexBuffer(unsigned int stream, const IBufferBase* buffer, unsigned long stride) {
    const OGLVertexBuffer* vertexBuffer = static_cast<const OGLVertexBuffer*> (buffer);

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertexBuffer->getBuffer());
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, stride, 0);

    glActiveTextureARB( GL_TEXTURE0_ARB);
    glClientActiveTextureARB(GL_TEXTURE0_ARB);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, stride, BUFFER_OFFSET( 12 ));
}

void OGLRenderer::setIndexBuffer(const IBufferBase* buffer) {
    const OGLIndexBuffer* indexBuffer = static_cast<const OGLIndexBuffer*> (buffer);

    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indexBuffer->getBuffer());
    glEnableClientState(GL_INDEX_ARRAY);
}

void OGLRenderer::drawPrimitives(TPrimitiveType meshType, unsigned long count) {
    switch (meshType) {
        case PT_TRIANGLELIST:
            glDrawArrays(GL_TRIANGLES, 0, count * 3);
            break;
        case PT_TRIANGLESTRIP:
            glDrawArrays(GL_TRIANGLE_STRIP, 0, count + 2);
            break;
        case PT_TRIANGLEFAN:
            glDrawArrays(GL_TRIANGLE_FAN, 0, count + 1);
            break;
        case PT_LINELIST:
            glDrawArrays(GL_LINES, 0, count * 2);
            break;
        case PT_LINESTRIP:
            glDrawArrays(GL_LINE_STRIP, 0, count + 1);
            break;
        case PT_POINTLIST:
            glDrawArrays(GL_POINTS, 0, count);
            break;
    }
}

void OGLRenderer::drawIndexedPrimitives(TPrimitiveType meshType, unsigned long count) {
    unsigned long indicesType = GL_UNSIGNED_SHORT;

    switch (meshType) {
        case PT_TRIANGLELIST:
            glDrawElements(GL_TRIANGLES, count * 3, indicesType, 0);
            break;
        case PT_TRIANGLESTRIP:
            glDrawElements(GL_TRIANGLE_STRIP, count + 2, indicesType, 0);
            break;
        case PT_TRIANGLEFAN:
            glDrawElements(GL_TRIANGLE_FAN, count + 1, indicesType, 0);
            break;
        case PT_LINELIST:
            glDrawElements(GL_LINES, count * 2, indicesType, 0);
            break;
        case PT_LINESTRIP:
            glDrawElements(GL_LINE_STRIP, count + 1, indicesType, 0);
            break;
        case PT_POINTLIST:
            glDrawElements(GL_POINTS, count, indicesType, 0);
            break;
    }

    //    glDisableClientState(GL_VERTEX_ARRAY);
    //    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    //    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
}

GLenum Get(const ::protocol::duke::SamplerState_Type type) {
    switch (type) {
        case ::protocol::duke::SamplerState_Type_MIN_FILTER:
            return GL_TEXTURE_MIN_FILTER;
        case ::protocol::duke::SamplerState_Type_MAG_FILTER:
            return GL_TEXTURE_MAG_FILTER;
        case ::protocol::duke::SamplerState_Type_WRAP_S:
            return GL_TEXTURE_WRAP_S;
        case ::protocol::duke::SamplerState_Type_WRAP_T:
            return GL_TEXTURE_WRAP_T;
    }
    std::cerr << "invalid sampler state type " << ::protocol::duke::SamplerState_Type_Name(type) << std::endl;
    assert(false);
    return 0;
}

GLenum Get(const ::protocol::duke::SamplerState_Value value) {
    switch (value) {
        case ::protocol::duke::SamplerState_Value_TEXF_POINT:
            return GL_NEAREST;
        case ::protocol::duke::SamplerState_Value_TEXF_LINEAR:
            return GL_LINEAR;
        case ::protocol::duke::SamplerState_Value_WRAP_CLAMP:
            return GL_CLAMP_TO_EDGE;
        case ::protocol::duke::SamplerState_Value_WRAP_BORDER:
            return GL_CLAMP_TO_BORDER;
        case ::protocol::duke::SamplerState_Value_WRAP_REPEAT:
            return GL_REPEAT;
    }
    std::cerr << "invalid sampler state value " << ::protocol::duke::SamplerState_Value_Name(value) << std::endl;
    assert(false);
    return 0;
}

inline static GLenum Get(const Effect_Blend &value) {
    switch (value) {
        case Effect_Blend_BLEND_ZERO:
            return GL_ZERO;
        case Effect_Blend_BLEND_ONE:
            return GL_ONE;
        case Effect_Blend_BLEND_SRCALPHA:
            return GL_SRC_ALPHA;
        case Effect_Blend_BLEND_INVSRCALPHA:
            return GL_ONE_MINUS_SRC_ALPHA;
        case Effect_Blend_BLEND_DESTALPHA:
            return GL_DST_ALPHA;
        case Effect_Blend_BLEND_INVDESTALPHA:
            return GL_ONE_MINUS_DST_ALPHA;
    }
    std::cerr << "invalid render state value " << Effect_Blend_Name(value) << std::endl;
    assert(false);
    return GL_ONE;
}

void OGLRenderer::setRenderState(const Effect &renderState) const {
    if (renderState.has_alphablend()) {
        if (renderState.alphablend())
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);
    }
    if (renderState.has_srcblend() && renderState.has_dstblend())
        glBlendFunc(Get(renderState.srcblend()), Get(renderState.dstblend()));
    if (renderState.has_alphasrcblend() && renderState.has_alphadstblend())
        glAlphaFunc(Get(renderState.alphasrcblend()), Get(renderState.alphadstblend()));
}

void OGLRenderer::setTexture(const CGparameter sampler, const RepeatedPtrField<SamplerState>& samplerStates, const ITextureBase* pTextureBase) const {
    const OGLTexture* oglTexture = static_cast<const OGLTexture*> (pTextureBase);

    GLenum textureType;
    if (oglTexture->getDepth() <= 1)
        textureType = GL_TEXTURE_2D;
    else
        textureType = GL_TEXTURE_3D;

    glBindTexture(textureType, oglTexture->getGLTexture());
    RepeatedPtrField<SamplerState>::const_iterator itr = samplerStates.begin();
    for (; itr != samplerStates.end(); ++itr)
        glTexParameteri(textureType, Get(itr->type()), Get(itr->value()));
}

void OGLRenderer::setShader(IShaderBase* shader) {
    OGLShader* glShader = static_cast<OGLShader*> (shader);

    if (glShader) {
        cgGLBindProgram(glShader->getProgram());
        cgGLEnableProfile(glShader->getType() == SHADER_PIXEL ? m_PSProfile : m_VSProfile);
    } else {
        cgGLDisableProfile(m_VSProfile);
        cgGLDisableProfile(m_PSProfile);
        cgGLUnbindProgram(m_VSProfile);
        cgGLUnbindProgram(m_PSProfile);
    }
}

void OGLRenderer::beginScene(bool shouldClean, uint32_t cleanColor, ITextureBase* pRenderTarget) {
    //FIXME: call glViewport only when we receive a resizeEvent
    glViewport(0, 0, m_Window.GetWidth(), m_Window.GetHeight());

    if (pRenderTarget) {
        assert( pRenderTarget->isRenderTarget() );
        OGLTexture* pTexture = dynamic_cast<OGLTexture*> (pRenderTarget);


        // Bind the framebuffer object
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_Fbo);
        // Attach a texture to the FBO
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, pTexture->getGLTexture(), 0);

        // For depth testing, create and attach a depth renderbuffer
        // Bind renderbuffer
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_RenderBuffer);
        // Init as a depth buffer
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, pTexture->getWidth(), pTexture->getHeight());
        // Attach to the FBO for depth
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_RenderBuffer);

        //Does the GPU support current FBO configuration?
        GLenum status;
        status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
        switch (status) {
            case GL_FRAMEBUFFER_COMPLETE_EXT:
                break;
            default:
                std::cerr << "GL error: FBO EXT not supported by GPU" << std::endl;
        }
    }

    if (shouldClean) {
        glClearColor(cleanColor >> 24, (cleanColor >> 16) & 0xFF, (cleanColor >> 8) & 0xFF, (cleanColor) & 0xFF);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
    glLoadIdentity();
}

void OGLRenderer::endScene() {
    // Unbind the FBO (and renderbuffer if necessary).
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
    glDeleteRenderbuffersEXT(1, &m_RenderBuffer);
    //Bind 0, which means render to back buffer, as a result, fb is unbound
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glDeleteFramebuffersEXT(1, &m_Fbo);
}

void OGLRenderer::presentFrame() {
    m_Window.Display();
}

void OGLRenderer::waitForBlanking() const {
}

Image OGLRenderer::dumpTexture(ITextureBase* pTextureBase) {
    assert( pTextureBase );

    OGLTexture* pTexture = dynamic_cast<OGLTexture*> (pTextureBase);
    ImageDescription description;

    description.width = pTexture->getWidth();
    description.height = pTexture->getHeight();
    description.format = pTexture->getFormat();
    description.depth = pTexture->getDepth();

    //    if (pTextureBase) {
    //        std::string filename = "output.ppm";
    //        bool fileExist = false;
    //        /*
    //         std::ifstream filein;
    //         filein.open(filename.c_str(), std::ios::in);
    //         if( filein.is_open() )
    //         fileExist = true;
    //         filein.close();
    //         */
    //        std::ofstream file(filename.c_str(), std::ios::out | std::ios::trunc);
    //        if (file && !fileExist) {
    //            std::cout << "rendering to file " << filename << std::endl;
    //
    //            // PPM header
    //            file << "P3" << std::endl; // ASCII mode
    //            file << description.width << " " << description.height << std::endl; // image width & height
    //            file << 255 << std::endl; // max color value
    //
    //            glGenBuffersARB(1, &m_Pbo);
    //            glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, m_Pbo);
    //            glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, description.width * description.height * 4, 0, GL_STREAM_READ_ARB);
    //
    //            //glReadBuffer(GL_FRONT);
    //            glReadPixels(0, 0, description.width, description.height, GL_BGRA, GL_UNSIGNED_BYTE, 0);
    //
    //            // map the PBO that contain framebuffer pixels before processing it
    //            GLubyte* src = (GLubyte*) glMapBufferARB(GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY_ARB);
    //            if (src) {
    //                //for (int i = 0; i < description.width * description.height * 4; i += 4) {
    //                //    file << (int) src[i] << " " << (int) src[i + 1] << " " << (int) src[i + 2] << " " /*<< (int) src[i + 3]*/<< std::endl;
    //                //}
    //
    //                for (int i = 0; i < description.width; ++i) {
    //                    for (int j = 0; j < description.height * 4; j += 4) {
    //                        file << (int) src[(i * description.height * 4) + j] << " " << (int) src[(i * description.height * 4) + j + 1] << " " << (int) src[(i * description.height
    //                                * 4) + j + 2] << std::endl;
    //                    }
    //                }
    //
    //                glUnmapBufferARB(GL_PIXEL_PACK_BUFFER_ARB); // release pointer to the mapped buffer
    //            }
    //            glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, 0);
    //            glDeleteBuffersARB(1, &m_Pbo);
    //
    //            file.close();
    //
    //        } else {
    //            std::cerr << "Unable to write file " << filename << std::endl;
    //        }
    //    }
    /*
     const GLenum errCode = glGetError();
     if (errCode != GL_NO_ERROR) {
     if (errCode == GL_INVALID_OPERATION)
     std::cerr << "GL_INVALID_OPERATION" << std::endl;
     if (errCode == GL_INVALID_VALUE)
     std::cerr << "GL_INVALID_VALUE" << std::endl;
     if (errCode == GL_INVALID_ENUM)
     std::cerr << "GL_INVALID_ENUM" << std::endl;
     }
     */
    Image image(*this, "", description);
    return image;
}

