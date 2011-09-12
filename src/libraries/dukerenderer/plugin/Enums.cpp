#include "Enums.h"
#include "utils/PixelUtils.h"

#include <stdexcept>
#include <iostream>

TPrimitiveType Enums::Get(const ::protocol::duke::Mesh_MeshType& type) {
    switch (type) {
        case ::protocol::duke::Mesh_MeshType_POINTLIST:
            return PT_POINTLIST;
        case ::protocol::duke::Mesh_MeshType_LINELIST:
            return PT_LINELIST;
        case ::protocol::duke::Mesh_MeshType_LINESTRIP:
            return PT_LINESTRIP;
        case ::protocol::duke::Mesh_MeshType_TRIANGLELIST:
            return PT_TRIANGLELIST;
        case ::protocol::duke::Mesh_MeshType_TRIANGLESTRIP:
            return PT_TRIANGLESTRIP;
        case ::protocol::duke::Mesh_MeshType_TRIANGLEFAN:
            return PT_TRIANGLEFAN;
        default:
            std::cerr << "invalid primitive type " << ::protocol::duke::Mesh_MeshType_Name(type) << std::endl;
    }
    return PT_POINTLIST;
}

TPixelFormat Enums::Get(const ::protocol::duke::Texture_TextureFormat& format) {
    switch (format) {
        case ::protocol::duke::Texture_TextureFormat_R8G8B8A8:
            return PXF_R8G8B8A8;
        case ::protocol::duke::Texture_TextureFormat_R32G32B32A32F:
            return PXF_R32G32B32A32F;
        case ::protocol::duke::Texture_TextureFormat_R10G10B10A2:
            return PXF_R10G10B10A2;
        case ::protocol::duke::Texture_TextureFormat_R32F:
            return PXF_R32F;
        default:
            std::cerr << "invalid pixel format " << ::protocol::duke::Texture_TextureFormat_Name(format) << std::endl;
    }
    return PXF_UNDEFINED;
}

::protocol::duke::Texture_TextureFormat Enums::Get(const TPixelFormat& format) {
    switch (format) {
        case PXF_R8G8B8A8:
            return ::protocol::duke::Texture_TextureFormat_R8G8B8A8;
        case PXF_R32G32B32A32F:
            return ::protocol::duke::Texture_TextureFormat_R32G32B32A32F;
        case PXF_R10G10B10A2:
            return ::protocol::duke::Texture_TextureFormat_R10G10B10A2;
        case PXF_R32F:
            return ::protocol::duke::Texture_TextureFormat_R32F;
        default:
            std::cerr << "invalid pixel format " << FormatToString(format) << std::endl;
    }
    return ::protocol::duke::Texture_TextureFormat_R8G8B8A8;
}
