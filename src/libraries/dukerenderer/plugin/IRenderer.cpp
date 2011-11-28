#include "IRenderer.h"
#include "VolatileTexture.h"
#include "DisplayableImage.h"
#include "ProtoBufResource.h"
#include "utils/SfmlProtobufUtils.h"
#include "ShaderFactory.h"

#include <dukeapi/serialize/ProtobufSerialize.h>

#include <boost/thread.hpp>
#include <boost/foreach.hpp>

#include <iostream>

using namespace ::duke::protocol;
using namespace ::google::protobuf;
using namespace ::std;

using namespace google::protobuf::serialize;

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
inline void IRenderer::addResource(const ::google::protobuf::serialize::MessageHolder& holder) {
    const T msg = unpackTo<T> (holder);
    getResourceManager().add( //
                             msg.name(), //
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

static void dump(const google::protobuf::Descriptor* pDescriptor, const google::protobuf::serialize::MessageHolder &holder) {
#ifdef DEBUG_MESSAGES
    cerr << HEADER + "pop " + pDescriptor->name() << "\t" << unpack(holder)->ShortDebugString() << endl;
#endif
}

void IRenderer::consumeUntilEngine() {
    // updating resources by popping all the pending messages
    const MessageHolder* pHolder;
    while ((pHolder = m_RendererSuite.popEvent()) != NULL) {
        const MessageHolder &holder(*pHolder);
        const Descriptor *pDescriptor(descriptorFor(holder));
        holder.CheckInitialized();
        dump(pDescriptor, holder);

        if (isType<Shader> (pDescriptor)) {
            const Shader shader = unpackTo<Shader> (holder); // fixme gchatelet : unpack to shared for addResource
            getResourceManager().remove(::resource::SHADER, shader.name());
            addResource<Shader> (holder);
        } else if (isType<duke::protocol::Mesh> (pDescriptor)) {
            using duke::protocol::Mesh;
            const Mesh mesh = unpackTo<Mesh> (holder); // fixme gchatelet : unpack to shared for addResource
            getResourceManager().remove(::resource::MESH, mesh.name());
            addResource<Mesh> (holder);
        } else if (isType<Texture> (pDescriptor)) {
            const Texture texture = unpackTo<Texture> (holder); // fixme gchatelet : unpack to shared for addResource
            switch (holder.action()) {
                case MessageHolder_Action_CREATE:
                case MessageHolder_Action_UPDATE:
                    DisplayableImage(*this, texture);
                    break;
                default: {
                    ostringstream msg;
                    msg << HEADER << "IRenderer : don't know how to action " << MessageHolder_Action_Name(holder.action()) << " on a Texture" << endl;
                    throw runtime_error(msg.str());
                }
            }
        } else if (isType<StaticParameter> (pDescriptor)) {
            addResource<StaticParameter> (holder);
        } else if (isType<AutomaticParameter> (pDescriptor)) {
            addResource<AutomaticParameter> (holder);
        } else if (isType<Grading> (pDescriptor)) {
            addResource<Grading> (holder);
        } else if (isType<Event> (pDescriptor)) {
            const Event event = unpackTo<Event> (holder);
            if (event.type() == Event_Type_RESIZED) {
                const ResizeEvent &resizeEvent = event.resizeevent();
                if (resizeEvent.has_height() && resizeEvent.has_width())
                    m_Window.SetSize(resizeEvent.width(), resizeEvent.height());
                if (resizeEvent.has_x() && resizeEvent.has_y())
                    m_Window.SetPosition(resizeEvent.x(), resizeEvent.y());
            }
        } else if (isType<FunctionPrototype> (pDescriptor)) {
            getPrototypeFactory().setPrototype(unpackTo<FunctionPrototype> (holder));
        } else if (isType<Engine> (pDescriptor)) {
            m_EngineStatus.CopyFrom(unpackTo<Engine> (holder));
            if (m_EngineStatus.action() != Engine_Action_RENDER_STOP)
                return;
        } else {
            ostringstream msg;
            msg << HEADER << "IRenderer : unknown message type " << pDescriptor->name() << endl;
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
                BOOST_FOREACH( const duke::protocol::Clip &clip, m_pSetup->m_Clips) {
                    displayClip(clip);
                }
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
    MessageHolder holder;
    try {
        Event event;
        while (m_Window.PollEvent(m_Event)) {
            event.Clear();
            // transcoding the event to protocol buffer
            Update(event, m_Event);
            pack(holder, event);
            m_RendererSuite.pushEvent(holder);
        }
    } catch (exception& e) {
        cerr << HEADER + "Unexpected error while dispatching events : " + e.what() << endl;
        return false;
    }

    BOOST_FOREACH ( const DumpedImages::value_type &pair, m_Context.dumpedImages) {
        const string &name = pair.first;
        Texture texture;
        pair.second->dump(texture);
        texture.set_name(name);
        pack(holder, texture);
        m_RendererSuite.pushEvent(holder);
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
            RenderTargets::const_iterator itr = m_Context.renderTargets.find(renderTargetName);
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
            BOOST_FOREACH( const string& name, pass.meshname()) {
                displayMesh(getResourceManager().safeGetProto<duke::protocol::Mesh> (name));
            }
        }

        // dumpTexture
        if (pass.has_grab()) {
            DumpedImages &images = m_Context.dumpedImages;
            if (pRenderTargetTexture) {
                images[pass.grab().name()].reset(new Image(dumpTexture(pRenderTargetTexture)));
            } else {
                cerr << "A grab is requested " << pass.name() << ", but render target is NULL" << endl;
            }
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
    const vector<ImageDescription> &images = m_pSetup->m_Images;
    if (clipName.empty())
        return getSafeImageDescription(images.empty() ? NULL : &images[0]);

    size_t index = 0;
    BOOST_FOREACH(const duke::protocol::Clip &clip ,m_pSetup->m_Clips) {
        if (clip.has_name() && clip.name() == clipName)
            return getSafeImageDescription(&images[index]);
        ++index;
    }

    cerr << HEADER + "no clip associated to " << clipName << endl;
    return m_EmptyImageDescription;
}
