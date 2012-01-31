#include "Enums.h"
#include "utils/PixelUtils.h"

#include <stdexcept>
#include <iostream>

TPrimitiveType Enums::Get(const ::duke::protocol::Mesh_MeshType& type) {
    switch (type) {
        case ::duke::protocol::Mesh_MeshType_POINTLIST:
            return PT_POINTLIST;
        case ::duke::protocol::Mesh_MeshType_LINELIST:
            return PT_LINELIST;
        case ::duke::protocol::Mesh_MeshType_LINESTRIP:
            return PT_LINESTRIP;
        case ::duke::protocol::Mesh_MeshType_TRIANGLELIST:
            return PT_TRIANGLELIST;
        case ::duke::protocol::Mesh_MeshType_TRIANGLESTRIP:
            return PT_TRIANGLESTRIP;
        case ::duke::protocol::Mesh_MeshType_TRIANGLEFAN:
            return PT_TRIANGLEFAN;
        default:
            std::cerr << "invalid primitive type " << ::duke::protocol::Mesh_MeshType_Name(type) << std::endl;
    }
    return PT_POINTLIST;
}

TPixelFormat Enums::Get(const ::duke::protocol::Texture_TextureFormat& format) {
    switch (format) {
        case ::duke::protocol::Texture_TextureFormat_R8G8B8A8:
            return PXF_R8G8B8A8;
        case ::duke::protocol::Texture_TextureFormat_R32G32B32A32F:
            return PXF_R32G32B32A32F;
        case ::duke::protocol::Texture_TextureFormat_R10G10B10A2:
            return PXF_R10G10B10A2;
        case ::duke::protocol::Texture_TextureFormat_R32F:
            return PXF_R32F;
        default:
            std::cerr << "invalid pixel format " << ::duke::protocol::Texture_TextureFormat_Name(format) << std::endl;
    }
    return PXF_UNDEFINED;
}

::duke::protocol::Texture_TextureFormat Enums::Get(const TPixelFormat& format) {
    switch (format) {
        case PXF_R8G8B8A8:
            return ::duke::protocol::Texture_TextureFormat_R8G8B8A8;
        case PXF_R32G32B32A32F:
            return ::duke::protocol::Texture_TextureFormat_R32G32B32A32F;
        case PXF_R10G10B10A2:
            return ::duke::protocol::Texture_TextureFormat_R10G10B10A2;
        case PXF_R32F:
            return ::duke::protocol::Texture_TextureFormat_R32F;
        default:
            std::cerr << "invalid pixel format " << FormatToString(format) << std::endl;
    }
    return ::duke::protocol::Texture_TextureFormat_R8G8B8A8;
}
