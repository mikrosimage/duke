#ifndef OGLRENDERER_H_
#define OGLRENDERER_H_

#include <GL/glew.h>
#include <dukeapi/protocol/player/communication.pb.h>
#include <plugin/RendererSuite.h>
#include <plugin/IRenderer.h>
#include <plugin/IFactory.h>
#include <plugin/Mesh.h>

class OGLRenderer : public IRenderer
{
protected:
	virtual void beginScene( bool shouldClean, uint32_t cleanColor, ITextureBase* pRenderTarget = NULL );
	virtual void endScene();
	virtual void presentFrame();
    virtual void waitForBlanking() const;
	virtual Image dumpTexture( ITextureBase* pTextureBase );

public: OGLRenderer( const protocol::duke::Renderer& Renderer, sf::Window& window, const RendererSuite& suite );
	~OGLRenderer();

	// IFactory
	virtual IBufferBase*  createVB( unsigned long size, unsigned long stride, unsigned long flags ) const;
	virtual IBufferBase*  createIB( unsigned long size, unsigned long stride, unsigned long flags ) const;
	virtual IShaderBase*  createShader( CGprogram program, TShaderType type ) const;
	virtual TPixelFormat  getCompliantFormat(TPixelFormat format) const;
	virtual ITextureBase* createTexture( const ImageDescription& description, long unsigned int ) const;
	virtual void          checkCaps();

	// IRenderer
	virtual void setShader( IShaderBase* shader );
	virtual void setVertexBuffer( unsigned int stream, const IBufferBase* buffer, unsigned long stride );
	virtual void setIndexBuffer( const IBufferBase* buffer );
	virtual void drawPrimitives( TPrimitiveType meshType, unsigned long count );
	virtual void drawIndexedPrimitives( TPrimitiveType meshType, unsigned long count );
    virtual void setRenderState( const ::protocol::duke::Effect &renderState ) const;
	virtual void setTexture( const CGparameter sampler, const ::google::protobuf::RepeatedPtrField< ::protocol::duke::SamplerState >& samplerStates, const ITextureBase* pTextureBase ) const;

    virtual GLuint getPBO() const {
        return m_Pbo;
    }

private:
	GLuint m_Fbo;
	GLuint m_RenderBuffer;
	GLuint m_Pbo;
};

#endif /* OGLRENDERER_H_ */
