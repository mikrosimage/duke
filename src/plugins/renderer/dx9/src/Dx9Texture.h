#ifndef DX9TEXTURE_H_
#define DX9TEXTURE_H_

#include <dukerenderer/plugin/math/Rect.h>
#include <dukerenderer/plugin/ITextureBase.h>
#include "ScopedIUnknownPtr.h"
#include <d3d9.h>

class Dx9Renderer;

class Dx9Texture : public ITextureBase
{
public:
	Dx9Texture( const ImageDescription& description, unsigned long usageFlag, const Dx9Renderer& renderer );

	IDirect3DTexture9* GetDxTexture() const;

private:
	Dx9Texture( const Dx9Texture& );
	virtual void update( const ImageDescription& description, const unsigned char* pData );

	ScopedIUnknownPtr<IDirect3DTexture9> m_pTexture;
};

#endif // DX9TEXTURE_H_
