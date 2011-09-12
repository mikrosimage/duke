#include "OGLEnum.h"

#include <plugin/utils/PixelUtils.h>

#include <iostream>
#include <stdexcept>
#include <cassert>

unsigned long OGLEnum::BufferFlags(unsigned long Flags) {
    return (Flags & BUF_DYNAMIC ? GL_STATIC_DRAW_ARB : GL_STATIC_DRAW_ARB);
}

unsigned long OGLEnum::LockFlags(unsigned long Flags) {
    unsigned long LockFlags = GL_READ_WRITE_ARB;

    if (Flags & LOCK_READONLY)
        LockFlags = GL_READ_ONLY_ARB;
    if (Flags & LOCK_WRITEONLY)
        LockFlags = GL_WRITE_ONLY_ARB;

    return LockFlags;
}

// Specifies the format of the pixel data.
GLint OGLEnum::GetFormat(TPixelFormat _format) {
    switch (_format) {
        case PXF_R8G8B8A8:
        case PXF_B8G8R8A8:
        case PXF_R10G10B10A2:
        case PXF_A2B10G10R10:
        case PXF_R16G16B16A16F:
        case PXF_B16G16R16A16F:
        case PXF_R32G32B32A32F:
        case PXF_B32G32R32A32F:
            return GL_BGRA;
        case PXF_R8G8B8:
        case PXF_B8G8R8:
        case PXF_R16G16B16F:
        case PXF_B16G16R16F:
        case PXF_R32G32B32F:
        case PXF_B32G32R32F:
            return GL_BGR;
        case PXF_R32F:
            return GL_R;
        case PXF_UNDEFINED:
            std::cerr << "Unsupported Format for OpenGL " << FormatToString(_format) << std::endl;
            break;
    }
    assert(false);
    return 0;
}

// Specifies the number of color components in the texture
GLint OGLEnum::GetInternalFormat(TPixelFormat _format) {
    switch (_format) {
//        case PXF_R32G32B32A32F:
//            return GL_RGBA32F_ARB;
//        case PXF_R16G16B16A16F:
//            return GL_RGBA16F_ARB;
        case PXF_R32F:
            return 1;
        case PXF_R8G8B8:
        case PXF_B8G8R8:
        case PXF_R16G16B16F:
        case PXF_B16G16R16F:
        case PXF_R32G32B32F:
        case PXF_B32G32R32F:
            return 3;
        case PXF_R8G8B8A8:
        case PXF_B8G8R8A8:
        case PXF_R10G10B10A2:
        case PXF_A2B10G10R10:
        case PXF_R16G16B16A16F:
        case PXF_B16G16R16A16F:
        case PXF_R32G32B32A32F:
        case PXF_B32G32R32A32F:
            return 4;
        case PXF_UNDEFINED:
        default:
            std::cerr << "Unsupported Internal Format for OpenGL " << FormatToString(_format) << std::endl;
            break;
    }
    assert(false);
    return 1;
}

// Specifies the data type of the pixel data.
GLint OGLEnum::GetType(TPixelFormat _format) {
    switch (_format) {
        case PXF_B8G8R8A8:
        case PXF_R8G8B8A8:
        case PXF_R8G8B8:
        case PXF_B8G8R8:
        case PXF_R10G10B10A2:
        case PXF_A2B10G10R10:
            return GL_UNSIGNED_BYTE;
        case PXF_R16G16B16A16F:
        case PXF_B16G16R16A16F:
        case PXF_R16G16B16F:
        case PXF_B16G16R16F:
            return GL_HALF_FLOAT_ARB;
        case PXF_R32G32B32A32F:
        case PXF_B32G32R32A32F:
        case PXF_R32G32B32F:
        case PXF_B32G32R32F:
        case PXF_R32F:
            return GL_FLOAT;
        case PXF_UNDEFINED:
            std::cerr << "Unsupported Format for OpenGL " << FormatToString(_format) << std::endl;
            break;
    }
    assert(false);
    return 0;
}

