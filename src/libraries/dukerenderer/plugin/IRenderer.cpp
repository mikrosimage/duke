#include "IRenderer.h"
#include "VolatileTexture.h"
#include "DisplayableImage.h"
#include "ProtoBufResource.h"
#include "utils/SfmlProtobufUtils.h"
#include "ShaderFactory.h"

#include <boost/thread.hpp>

#include <iostream>

using namespace ::duke::protocol;
using namespace ::google::protobuf;
using namespace ::std;

const char emptyImageData[] = { CHAR_MAX, CHAR_MIN, CHAR_MAX, CHAR_MIN };
const string HEADER = "[IRenderer] ";

/**
 * RAII to enforce end scene is paired with begin scene
 */
struct RAIIScene {
    RAIIScene(IRenderer& renderer, bool shouldClean, uint32_t cleanColor, ITextureBase* pRenderTarget = NULL) :
        m_Renderer(renderer) {
        m_Renderer.beginScene(shouldClean, cleanColor, pRenderTarget);
    }
    ~RAIIScene() {
        m_Renderer.endScene();
        m_Renderer.m_bRenderOccured = true;
    }
private:
    IRenderer &m_Renderer;
};

/**
 * RAII to enforce context is popped if pushed
 */
struct RAIIContext {
    RAIIContext(RenderingContext& context, const string& name, const bool doPush) :
        m_Scopes(context.scopes), m_doPush(doPush) {
        if (m_doPush)
            m_Scopes.push_back(name);
    }
    ~RAIIContext() {
        if (m_doPush)
            m_Scopes.pop_back();
    }
private:
    Scopes &m_Scopes;
    const bool m_doPush;
};

/**
 * templated helper to add a protobuf resource in the resource manager
 */
template<typename T>
inline void IRenderer::addResource(const ::google::protobuf::Message& msg) {
    getResourceManager().add( //
                             dynamic_cast<const T&> (msg).name(), //
                             new ProtoBufResource(msg), //
                             false //
    );
}

IRenderer::IRenderer(const duke::protocol::Renderer& renderer, sf::Window& window, const RendererSuite& suite) :
    m_Window(window), m_Renderer(renderer), m_RendererSuite(suite), m_pSetup(NULL), m_DisplayedFrameCount(0), m_bRenderOccured(false) {
    m_EmptyImageDescription.width = 1;
    m_EmptyImageDescription.height = 1;
    m_EmptyImageDescription.depth = 0;
    m_EmptyImageDescription.pImageData = emptyImageData;
    m_EmptyImageDescription.imageDataSize = sizeof(emptyImageData);
    m_EmptyImageDescription.format = PXF_R8G8B8A8;
    m_EngineStatus.set_action(Engine_Action_RENDER_START);
}

IRenderer::~IRenderer() {
}

void IRenderer::loop() {
    bool bLastFrame = false;

    // wait for the next message in the queue, store the result in 'msg'
    while (!bLastFrame) {
        // calling a simulation step
        try {
            bLastFrame = simulationStep();
        } catch (exception& e) {
            cerr << HEADER + "Unexpected error : " + e.what() << endl;
            boost::this_thread::sleep(boost::posix_time::millisec(200));
        } catch (...) {
            cerr << HEADER + "Unexpected error." << endl;
        }
        ++m_DisplayedFrameCount;
    }
}

void IRenderer::consumeUntilEngine() {
    // updating resources by popping all the pending messages
    MessageType msgType;
    const Message* pMessage;
    while ((pMessage = m_RendererSuite.popEvent(msgType)) != NULL) {
        const Message &msg(*pMessage);
        msg.CheckInitialized();
#ifdef DEBUG_MESSAGES
        cerr << HEADER + "pop " + MessageType_Type_Name(msgType.type()) << "\t" << msg.ShortDebugString() << endl;
#endif
        switch (msgType.type()) {
            case MessageType_Type_SHADER: {
                const Shader &s = dynamic_cast<const Shader&> (msg);
                getResourceManager().remove(::resource::SHADER, s.name());
                addResource<Shader> (msg);
                break;
            }
            case MessageType_Type_MESH: {
                const duke::protocol::Mesh& s = dynamic_cast<const duke::protocol::Mesh&> (msg);
                getResourceManager().remove(::resource::MESH, s.name());
                addResource<duke::protocol::Mesh> (msg);
                break;
            }
            case MessageType_Type_TEXTURE: {
                const Texture& texture = dynamic_cast<const Texture&> (msg);
                switch (msgType.action()) {
                    case MessageType_Action_SET:
                        DisplayableImage(*this, texture);
                        break;
                    default: {
                        ostringstream msg;
                        msg << HEADER << "IRenderer : don't know how to action " << MessageType_Action_Name(msgType.action()) << " on a Texture" << endl;
                        throw runtime_error(msg.str());
                    }
                }
                break;
            }
            case MessageType_Type_STATIC_PARAMETER:
                addResource<StaticParameter> (msg);
                break;
            case MessageType_Type_AUTOMATIC_PARAMETER:
                addResource<AutomaticParameter> (msg);
                break;
            case MessageType_Type_GRADING:
                addResource<Grading> (msg);
                break;
            case MessageType_Type_EVENT: {
                const Event &event = dynamic_cast<const Event&> (msg);
                switch (event.type()) {
                    case Event_Type_RESIZED: {
                        const ResizeEvent &resizeEvent = event.resizeevent();
                        if (resizeEvent.has_height() && resizeEvent.has_width())
                            m_Window.SetSize(resizeEvent.width(), resizeEvent.height());
                        if (resizeEvent.has_x() && resizeEvent.has_y())
                            m_Window.SetPosition(resizeEvent.x(), resizeEvent.y());
                        break;
                    }
                    default:
                        break;
                }
                break;
            }
            case MessageType_Type_SHADING_FUNCTION: {
                FunctionPrototype function;
                function.CopyFrom(msg);
                getPrototypeFactory().setPrototype(function);
                break;
            }
            case MessageType_Type_ENGINE: {
                m_EngineStatus.CopyFrom(msg);
                if (m_EngineStatus.action() != Engine_Action_RENDER_STOP)
                    return;
                break;
            }
            default:
                ostringstream msg;
                msg << HEADER << "IRenderer : unknown message type " << MessageType_Type_Name(msgType.type()) << endl;
                throw runtime_error(msg.str());
        }
    }
}

bool IRenderer::simulationStep() {
    m_Context.reset();
    m_RendererSuite.renderStart();

    try {
        consumeUntilEngine();
    } catch (exception& e) {
        cerr << HEADER + "Unexpected error while consuming messages in the rendering thread : " + e.what() << endl;
        return false;
    }

    // rendering clips
    {
        m_pSetup = &m_RendererSuite.getSetup();
        const bool renderRequested = m_EngineStatus.action() != Engine_Action_RENDER_STOP;
        const bool renderAvailable = m_pSetup && renderRequested && m_Window.IsOpened();
        if (renderAvailable) {
            try {
                m_bRenderOccured = false;
                for (auto itr = m_pSetup->m_Clips.begin(); itr != m_pSetup->m_Clips.end(); ++itr)
                    displayClip(*itr);
                if (!m_bRenderOccured) {
                    ::boost::this_thread::sleep(::boost::posix_time::milliseconds(10));
                } else {
                    OfxRendererSuiteV1::PresentStatus status;
                    while ((status = m_RendererSuite.getPresentStatus()) == OfxRendererSuiteV1::SKIP_NEXT_BLANKING)
                        waitForBlankingAndWarn(false);
                    presentFrame();
                    if (status == OfxRendererSuiteV1::PRESENT_NEXT_BLANKING)
                        waitForBlankingAndWarn(true);
                }
            } catch (exception& e) {
                cerr << HEADER + "Unexpected error while rendering : " + e.what() << endl;
                return false;
            }
        }

        if (m_EngineStatus.action() == Engine_Action_RENDER_ONE)
            m_EngineStatus.set_action(Engine_Action_RENDER_STOP);

        m_pSetup = NULL;
    }

    // Sending back messages if needed
    try {
        while (m_Window.PollEvent(m_Event)) {
            unique_ptr<Event> pEvent(new Event());
            pEvent->Clear();
            // transcoding the event to protocol buffer
            Update(*pEvent, m_Event);
            unique_ptr<Message> msg(pEvent.release());
            m_RendererSuite.pushEvent(msg);
        }
    } catch (exception& e) {
        cerr << HEADER + "Unexpected error while dispatching events : " + e.what() << endl;
        return false;
    }

    for (auto itr = m_Context.dumpedImages.begin(); itr != m_Context.dumpedImages.end(); ++itr) {
        const string &name = itr->first;
        unique_ptr<Texture> pTexture(new Texture());
        itr->second->dump(*pTexture);
        pTexture->set_name(name);
        unique_ptr<Message> msg(pTexture.release());
        m_RendererSuite.pushEvent(msg);
    }
    return m_RendererSuite.renderEnd(0);
}

void IRenderer::waitForBlankingAndWarn(bool presented) const {
    waitForBlanking();
    m_RendererSuite.verticalBlanking(presented);
}

void IRenderer::displayClip(const ::duke::protocol::Clip& clip) {
    try {
        if (!clip.has_grade() && !clip.has_gradename()) {
            cerr << HEADER + "no grading associated with clip" << endl;
            return;
        }
        if (clip.has_grade() && clip.has_gradename())
            cerr << HEADER + "clip has both grade and gradeName set : picking grade" << endl;

        // allocating context
        m_Context.set(m_pSetup->m_Images, m_DisplayedFrameCount, m_Window.GetWidth(), m_Window.GetHeight());
        RAIIContext clipContext(m_Context, clip.name(), clip.has_name());

        TResourcePtr pResource;
        const ::duke::protocol::Grading * pGrading = NULL;
        if (clip.has_grade()) {
            pGrading = &clip.grade();
        } else {
            pResource = getResourceManager().safeGet<ProtoBufResource> (::resource::PROTOBUF, clip.gradename());
            pGrading = pResource->get<duke::protocol::Grading> ();
        }
        RAIIContext gradingContext(m_Context, pGrading->name(), pGrading->has_name());
        for (int passIndex = 0; passIndex < pGrading->pass_size(); ++passIndex)
            displayPass(pGrading->pass(passIndex));
    } catch (exception &ex) {
        cerr << HEADER + ex.what() << " occurred while displaying clip " << clip.DebugString() << endl;
    }
}

static void overrideClipDimension(ImageDescription &description, const Texture &texture) {
    const bool hasWidth = texture.has_width();
    const bool hasHeight = texture.has_height();
    const bool hasNone = !hasWidth && !hasHeight;
    if (hasNone)
        return;
    const bool hasBoth = hasWidth && hasHeight;
    if (hasBoth) {
        description.width = texture.width();
        description.height = texture.height();
    } else {
        size_t newWidth;
        size_t newHeight;
        if (hasWidth) {
            newWidth = texture.width();
            newHeight = newWidth * description.height / description.width;
        } else {
            newHeight = texture.height();
            newWidth = newHeight * description.width / description.height;
        }
        description.width = newWidth;
        description.height = newHeight;
    }
}

void IRenderer::displayPass(const ::duke::protocol::RenderPass& pass) {
    try {
        // fetching the effect
        RAIIContext passContext(m_Context, pass.name(), pass.has_name());

        // preparing render target
        TexturePtr pRenderTarget;
        ITextureBase *pRenderTargetTexture = NULL;
        if (pass.target() == RenderPass_RenderTarget_TEXTURETARG) {
            assert(pass.has_rendertargetname());
            const string &renderTargetName = pass.rendertargetname();
            auto itr = m_Context.renderTargets.find(renderTargetName);
            if (itr != m_Context.renderTargets.end()) {
                pRenderTarget = itr->second;
            } else {
                assert(pass.has_rendertargetdimfromclipname());
                const string& clipName = pass.rendertargetdimfromclipname();
                ImageDescription clipDescription = getImageDescriptionFromClip(clipName);
                if (pass.has_rendertarget())
                    overrideClipDimension(clipDescription, pass.rendertarget());
                pRenderTarget.reset(new VolatileTexture(*this, clipDescription, TEX_RENTERTARGET));
                m_Context.renderTargets[renderTargetName] = pRenderTarget;
            }
            assert(pRenderTarget);
            pRenderTargetTexture = pRenderTarget->getTexture();
            //            cout << "rendering to " << renderTargetName << " : " << pRenderTargetTexture << endl;
        }

        // setting render target dimensions
        if (pRenderTargetTexture)
            m_Context.setRenderTarget(pRenderTargetTexture->getWidth(), pRenderTargetTexture->getHeight());
        else
            m_Context.setRenderTarget(m_Window.GetWidth(), m_Window.GetHeight());

        if (pass.has_effect() && pass.meshname_size() != 0) {
            RAIIScene scenePass(*this, pass.clean(), pass.cleancolor(), pRenderTargetTexture);

            const Effect& effect = pass.effect();
            // setting render state
            setRenderState(effect);

            // setting shaders
            assert(effect.has_vertexshadername());
            compileAndSetShader(SHADER_VERTEX, effect.vertexshadername());
            assert(effect.has_pixelshadername());
            compileAndSetShader(SHADER_PIXEL, effect.pixelshadername());

            // render all meshes
            for (auto itr = pass.meshname().begin(); itr != pass.meshname().end(); ++itr)
                displayMesh(getResourceManager().safeGetProto<duke::protocol::Mesh> (*itr));
        }

        // dumpTexture
        if (pRenderTargetTexture && pass.has_grab()) {
            auto &images = m_Context.dumpedImages;
            images[pass.grab().name()].reset(new Image(dumpTexture(pRenderTargetTexture)));
        }

    } catch (exception &ex) {
        cerr << HEADER + ex.what() << " occurred while displaying pass " << pass.DebugString() << endl;
    }
}

void IRenderer::displayMesh(const ::duke::protocol::Mesh& mesh) {
    ::Mesh(*this, mesh).render(*this);
}

void IRenderer::compileAndSetShader(const TShaderType& type, const string& name) {
    ShaderFactory(*this, getResourceManager().safeGetProto<Shader> (name), m_Context, type);
}

inline const ImageDescription& IRenderer::getSafeImageDescription(const ImageDescription* pImage) const {
    return (pImage == NULL || pImage->blank()) ? m_EmptyImageDescription : *pImage;
}

const ImageDescription& IRenderer::getImageDescriptionFromClip(const string &clipName) const {
    auto &images = m_pSetup->m_Images;
    if (clipName.empty())
        return getSafeImageDescription(images.empty() ? NULL : &images[0]);

    size_t index = 0;
    for (auto itr = m_pSetup->m_Clips.begin(); itr != m_pSetup->m_Clips.end(); ++itr, ++index)
        if (itr->has_name() && itr->name() == clipName)
            return getSafeImageDescription(&images[index]);

    cerr << HEADER + "no clip associated to " << clipName << endl;
    return m_EmptyImageDescription;
}
