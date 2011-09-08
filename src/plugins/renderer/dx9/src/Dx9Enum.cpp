/*
 * Dx9Enum.cpp
 *
 *  Created on: 17 mai 2010
 *      Author: Guillaume Chatelet
 */

#include "DX9Enum.h"

#include <renderer/plugin/common/Enums.h>
#include <renderer/plugin/common/utils/PixelUtils.h>

#include <iostream>
#include <stdexcept>
#include <cassert>

#include <d3d9.h>

unsigned long Dx9Enum::BufferFlags(unsigned long flags) {
    return (flags & BUF_DYNAMIC ? D3DUSAGE_DYNAMIC : 0);
}

unsigned long Dx9Enum::LockFlags(unsigned long flags) {
    unsigned long lockFlags = 0;

    if (flags & LOCK_READONLY)
        lockFlags = D3DLOCK_READONLY;

    if (flags & LOCK_WRITEONLY) {
        lockFlags = D3DLOCK_DISCARD;
        //LockFlags = D3DLOCK_NOOVERWRITE;
    }

    return lockFlags;
}

D3DPRIMITIVETYPE Dx9Enum::Get(TPrimitiveType type) {
    switch (type) {
        case PT_POINTLIST:
            return D3DPT_POINTLIST;
        case PT_LINELIST:
            return D3DPT_LINELIST;
        case PT_LINESTRIP:
            return D3DPT_LINESTRIP;
        case PT_TRIANGLELIST:
            return D3DPT_TRIANGLELIST;
        case PT_TRIANGLESTRIP:
            return D3DPT_TRIANGLESTRIP;
        case PT_TRIANGLEFAN:
            return D3DPT_TRIANGLEFAN;
        default:
            throw std::runtime_error("undefined primitive type");
    }
}

D3DFORMAT Dx9Enum::Get(TPixelFormat format) {
    switch (format) {
        case PXF_B8G8R8A8:
            return D3DFMT_A8B8G8R8;
        case PXF_R32G32B32A32F:
            return D3DFMT_A32B32G32R32F;
        case PXF_R16G16B16A16F:
            return D3DFMT_A16B16G16R16F;
        case PXF_R32F:
            return D3DFMT_R32F;
        case PXF_R10G10B10A2:
        case PXF_A2B10G10R10:
        case PXF_R8G8B8A8:
        case PXF_R8G8B8:
        case PXF_B8G8R8:
        case PXF_UNDEFINED:
            std::cerr << "Unsupported Format for DirectX9 \"" << FormatToString(format) << "\" " << format << std::endl;
            break;
    }
    assert(false);
    return D3DFMT_A8R8G8B8;
}

