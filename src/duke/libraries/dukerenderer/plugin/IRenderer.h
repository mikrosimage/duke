#ifndef IRENDERER_H_
#define IRENDERER_H_

#include "Enums.h"
#include "Mesh.h"
#include "IFactory.h"
#include "Image.h"
#include "RenderingContext.h"
#include "IShaderBase.h"

#include "RendererSuite.h"
#include <dukerenderer/Setup.h>

#include <player.pb.h>

#include <SFML/System.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Window.hpp>

class ITextureBase;
class IBufferBase;

class IRenderer : public IFactory {
public:
    IRenderer(const duke::protocol::Renderer&, sf::Window&, const RendererSuite&);
    virtual ~IRenderer();

    // to implement in derived classes
    virtual void setShader(IShaderBase* shader) = 0;
    virtual void setVertexBuffer(unsigned int stream, const IBufferBase* buffer, unsigned long stride) = 0;
    virtual void setIndexBuffer(const IBufferBase* buffer) = 0;
    virtual void drawPrimitives(TPrimitiveType meshType, unsigned long count) = 0;
    virtual void drawIndexedPrimitives(TPrimitiveType meshType, unsigned long count) = 0;
    virtual void setRenderState(const ::duke::protocol::Effect &renderState) const = 0;
    virtual void setTexture(const CGparameter sampler, //
                            const ::google::protobuf::RepeatedPtrField<duke::protocol::SamplerState>& samplerStates, //
                            const ITextureBase* pTextureBase) const = 0;

    virtual Image dumpTexture(ITextureBase* pTextureBase) = 0;
    virtual void waitForBlanking() const = 0;
    virtual void presentFrame() = 0;

protected:
    friend struct RAIIScene;
    // to implement in derived classes
    virtual void beginScene(bool shouldClean, uint32_t cleanColor, ITextureBase* pRenderTarget = NULL) = 0;
    virtual void endScene() = 0;

    // executes a simulation step : implements frame logic
    bool simulationStep();

    sf::Window& m_Window;

private:
    friend class SfmlWindow;
    void waitForBlankingAndWarn(bool presented) const;
    void consumeUntilEngine();
    void loop();
    void displayClip(const ::duke::protocol::Clip&);
    void displayPass(const ::duke::protocol::RenderPass&);
    void displayMesh(const ::duke::protocol::Mesh&);
    void displayMeshWithName(const std::string& name);
    void compileAndSetShader(const TShaderType&, const std::string&);
    friend class ShaderFactory;
    const ImageDescription& getSafeImageDescription(const ImageDescription*) const;
    const ImageDescription& getImageDescriptionFromClip(const std::string &clipName) const;

    template<typename T>
    inline void addResource(const ::google::protobuf::serialize::MessageHolder&);

    const duke::protocol::Renderer m_Renderer;
    const RendererSuite& m_RendererSuite;
    const Setup* m_pSetup;
    sf::Event m_Event;
    unsigned long m_DisplayedFrameCount;
    ::duke::protocol::Engine m_EngineStatus;
    ImageDescription m_EmptyImageDescription;
    bool m_bRenderOccured;
    RenderingContext m_Context;
};

#endif /* IRENDERER_H_ */
