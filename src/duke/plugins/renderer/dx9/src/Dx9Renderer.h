#ifndef DX9RENDERER_H_
#define DX9RENDERER_H_

#include "ScopedIUnknownPtr.h"
#include <player.pb.h>
#include <dukerenderer/plugin/RendererSuite.h>
#include <dukerenderer/plugin/IRenderer.h>
#include <dukerenderer/plugin/IFactory.h>
#include <d3d9.h>
#include <windows.h>

namespace sf {
class Window;
} // namespace sf

class ITextureBase;
class IBufferBase;

class Dx9Renderer : public IRenderer
{
private:
	ScopedIUnknownPtr<IDirect3D9> m_pD3d; // the pointer to our Direct3D interface
	ScopedIUnknownPtr<IDirect3DDevice9> m_pD3dDevice; // the pointer to the device class
	unsigned long m_iVertexCount;
	ScopedIUnknownPtr<IDirect3DSurface9> m_pBackBuffer; // the back buffer in case we're switching to render target
	ScopedIUnknownPtr<IDirect3DSurface9> m_pRenderSurface; // render surface associated with the texture render target
	bool m_bEndSceneCalled; // check that endScene() has been called before beginScene()

protected:
	virtual void beginScene( bool shouldClean, uint32_t cleanColor, ITextureBase* pRenderTarget = NULL );
	virtual void endScene();
	virtual void presentFrame();
    virtual void waitForBlanking() const;
	virtual Image dumpTexture( ITextureBase* pTextureBase );
	virtual void checkCaps();

public: Dx9Renderer( const duke::protocol::Renderer& Renderer, sf::Window& window, const RendererSuite& suite );
	virtual ~Dx9Renderer();

	// IFactory
	virtual IBufferBase*  createVB( unsigned long size, unsigned long stride, unsigned long flags ) const;
	virtual IBufferBase*  createIB( unsigned long size, unsigned long stride, unsigned long flags ) const;
	virtual IShaderBase*  createShader( CGprogram program, TShaderType type ) const;
	virtual TPixelFormat getCompliantFormat(TPixelFormat format) const;
	virtual ITextureBase* createTexture( const ImageDescription& description, unsigned long flags = 0 ) const;

	// IRenderer
	virtual void setShader( IShaderBase* shader );
	virtual void setVertexBuffer( unsigned int stream, const IBufferBase* buffer, unsigned long stride );
	virtual void setIndexBuffer( const IBufferBase* buffer );
	virtual void drawPrimitives( TPrimitiveType meshType, unsigned long count );
	virtual void drawIndexedPrimitives( TPrimitiveType meshType, unsigned long count );
    virtual void setRenderState( const ::duke::protocol::Effect &renderState ) const;
	virtual void setTexture( const CGparameter sampler, const ::google::protobuf::RepeatedPtrField< ::duke::protocol::SamplerState >& samplerStates, const ITextureBase* pTextureBase ) const;

	LPDIRECT3D9       getD3dInterface() const { return m_pD3d; }
	LPDIRECT3DDEVICE9 getD3dDevice() const    { return m_pD3dDevice; }
};

#endif /* DX9RENDERER_H_ */
