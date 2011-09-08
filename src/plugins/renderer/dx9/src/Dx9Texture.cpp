#include "Dx9Texture.h"
#include "Dx9Enum.h"
#include "Dx9Renderer.h"
#include "Dx9Utils.h"
#include <renderer/plugin/common/utils/PixelUtils.h>
#include <renderer/common/Setup.h>
#include <cassert>
#include <d3dx9tex.h>

using namespace std;

Dx9Texture::Dx9Texture(const ImageDescription& description, unsigned long usageFlag, const Dx9Renderer& renderer) :
    ITextureBase(description, renderer.getCompliantFormat(description.format), usageFlag) {
    const D3DFORMAT dxFormat = Dx9Enum::Get(getFormat());
    //	cerr << "Dx9Texture description.texture=" <<  FormatToString(m_Description.format) << endl;
    //	cerr << "Dx9Texture getFormat=" <<  FormatToString(getFormat()) << endl;
    //	cerr << "Dx9Texture dx9 texture=" <<  dxFormat << endl;
    const bool autoMipmapCapable = renderer.hasCapability(CAP_HW_MIPMAPPING) && //
            renderer.getD3dInterface()->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, D3DUSAGE_AUTOGENMIPMAP, D3DRTYPE_TEXTURE, dxFormat) == D3D_OK;

    unsigned long usage = 0;

    if (isMipMap() && isAutoMipMap() && autoMipmapCapable) {
        usage |= D3DUSAGE_AUTOGENMIPMAP;
        if (isRenderTarget())
            throw runtime_error("Cannot have TEX_AUTOMIPMAP && TEX_RENTERTARGET at the same time");
    }
    if (isRenderTarget()) {
        usage |= D3DUSAGE_RENDERTARGET;
        //usage |= D3DUSAGE_DYNAMIC;
    }

    if (description.depth > 1)
        throw runtime_error("3D textures not yet implemented with dx9 renderer");

    CHECK( D3DXCreateTexture( renderer.getD3dDevice(), //
                    getWidth(), // Width
                    getHeight(), // Height
                    isMipMap() ? D3DX_DEFAULT : 1, // MipLevels
                    usage, // Usage
                    dxFormat, // Format
                    isRenderTarget() ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, // Pool
                    &m_pTexture ) );
}

IDirect3DTexture9* Dx9Texture::GetDxTexture() const {
    return m_pTexture;
}

void Dx9Texture::update(const ImageDescription& description, const unsigned char* pData) {
    const int width = description.width;
    const int height = description.height;
    //    const int depth = description.depth;

    //	if (m_Format != format)
    //		throw runtime_error("cannot update texture, format doesn't match");

    if (description.depth > 1)
        throw runtime_error("3D textures not yet implemented with dx9 renderer");

    D3DLOCKED_RECT lockedRect;
    RAIITextureLock lock(m_pTexture, lockedRect, NULL, 0);

    const int imageLineSize = width * GetBytesPerPixel(getFormat());
    const int dataSize = imageLineSize * height;
    assert( lockedRect.Pitch >= imageLineSize );

    if (lockedRect.Pitch == imageLineSize) {
        memcpy(lockedRect.pBits, pData, dataSize);
    } else {
        unsigned char* pDest = reinterpret_cast<unsigned char*> (lockedRect.pBits);
        const unsigned char* pSrc = reinterpret_cast<const unsigned char*> (pData);
        for (int row = 0; row < height; ++row) {
            memcpy(pDest, pSrc, imageLineSize);
            pSrc += imageLineSize;
            pDest += lockedRect.Pitch;
        }
    }

    if (isMipMap()) {
        if (isAutoMipMap())
            m_pTexture->GenerateMipSubLevels();
        else
            D3DXFilterTexture(m_pTexture, NULL, D3DX_DEFAULT, D3DX_DEFAULT);
    }
}

