#include "ShaderFactory.h"
#include <dukeapi/protocol/player/ShadingGraphCodeBuilder.h>
#include "ProtoBufResource.h"
#include "IRenderer.h"
#include "DisplayableImage.h"
#include "VolatileTexture.h"

#include "utils/PixelUtils.h"

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <google/protobuf/descriptor.h>
#include <sstream>
#include <string>

using namespace ::std;
using namespace ::protocol::shader_assembler;
using namespace ::google::protobuf;
using namespace ::protocol::duke;
using namespace ::shader_factory;

ShaderFactory::ShaderFactory(IRenderer& renderer, const ::protocol::duke::Shader& shader, RenderingContext& context, const TShaderType type) :
    m_Renderer(renderer), m_Shader(shader), m_RenderingContext(context), m_Images(context.images()), m_Type(type), m_ResourceManager(m_Renderer.getResourceManager()) {
    const string name = m_Shader.name();
    const bool isPersistent = !name.empty();

    if (isPersistent)
        m_pShader = m_ResourceManager.get<IShaderBase> (::resource::SHADER, name);
    if (!m_pShader) {
        string code;
        if (m_Type == SHADER_VERTEX || shader.has_code())
            code = shader.code();
        else {
            // shader must have either a shading tree or code
            assert(shader.has_program());
            code = shader_factory::compile(shader.program(), renderer.getPrototypeFactory());
        }
        m_pShader.reset(renderer.createShader(createProgram(code, name), m_Type));
        if (isPersistent)
            m_ResourceManager.add(name, m_pShader);
    }
    assert( m_pShader->getType() == m_Type );
//    cout << "set shader with name " << name << endl;
    m_Renderer.setShader(m_pShader.get());
    applyParameters();
}

void ShaderFactory::applyParameters() {
    assert( m_pShader );
    const vector<string> &params = m_pShader->getParameterNames();
    for (vector<string>::const_iterator itr = params.begin(); itr != params.end(); ++itr)
        applyParameter(*itr);
}

TResourcePtr ShaderFactory::getParam(const string &name) const {
    for (ScopesRItr it = m_RenderingContext.scopes.rbegin(); it < m_RenderingContext.scopes.rend(); ++it) {
        const string scopedParamName = *it + "|" + name;
        TResourcePtr pParam = m_ResourceManager.get<ProtoBufResource> (::resource::PROTOBUF, scopedParamName);
        if (pParam != NULL)
            return pParam;
    }
    return m_ResourceManager.safeGet<ProtoBufResource> (::resource::PROTOBUF, name);
}

void ShaderFactory::applyParameter(const string& paramName) {
    const TResourcePtr pParam = getParam(paramName);
    const Descriptor* pDescriptor = pParam->getRef<Message> ().GetDescriptor();

    if (pDescriptor == StaticParameter::descriptor())
        applyParameter(paramName, pParam->getRef<StaticParameter> ());
    else if (pDescriptor == AutomaticParameter::descriptor())
        applyParameter(paramName, pParam->getRef<AutomaticParameter> ());
    else {
        cerr << "got unknown parameter type named : " << endl;
        pParam->getRef<Message> ().PrintDebugString();
    }
}

void ShaderFactory::applyParameter(const string& paramName, const AutomaticParameter& param) {
    float data[3] = { 1.f, 1.f, 1.f };
    switch (param.type()) {
        case AutomaticParameter_Type_FLOAT3_TEX_DIM: {
            ImageDescription imageDescriptionHolder;
            const ImageDescription *pImageDescription = NULL;
            if (param.has_samplingsource()) {
                const SamplingSource &samplingSource = param.samplingsource();
                assert(samplingSource.has_name());
                assert(samplingSource.has_type());
                const string &sourceName = samplingSource.name();
                switch (samplingSource.type()) {
                    case SamplingSource_Type_CLIP: {
                        pImageDescription = &m_Renderer.getImageDescriptionFromClip(sourceName);
                        break;
                    }
                    case SamplingSource_Type_SUPPLIED: {
                        const DisplayableImage image(m_Renderer, sourceName);
                        imageDescriptionHolder = image.getImageDescription();
                        pImageDescription = &imageDescriptionHolder;
                        break;
                    }
                    case SamplingSource_Type_SURFACE: {
                        RenderTargets::const_iterator itr = m_RenderingContext.renderTargets.find(sourceName);
                        if (itr != m_RenderingContext.renderTargets.end())
                            pImageDescription = &(itr->second->getImageDescription());
                        break;
                    }
                    default:
                        assert(!"not yet implemented");
                }
            }
            data[0] = pImageDescription ? pImageDescription->width : m_RenderingContext.renderTargetWidth();
            data[1] = pImageDescription ? pImageDescription->height : m_RenderingContext.renderTargetHeight();
            data[2] = data[0] / data[1];
//            cout << "setting " << paramName << " to " << data[0] << " " << data[1] << " " << data[2] << endl;
            break;
        }
        case AutomaticParameter_Type_FLOAT3_TIME: {
            data[0] = m_RenderingContext.displayedFrameCount();
            data[1] = 0;
            data[2] = 0;
            break;
        }
        default:
            assert(!"not yet implemented");
    }
    m_pShader->setParameter(paramName, data, sizeof(data) / sizeof(float));
}

void ShaderFactory::applyParameter(const string& paramName, const StaticParameter& param) {
    switch (param.type()) {
        case StaticParameter_Type_FLOAT:
            m_pShader->setParameter(paramName, param.floatvalue().data(), param.floatvalue_size());
            break;
        case StaticParameter_Type_SAMPLER: {
            assert(param.has_samplingsource());
            const SamplingSource &samplingSource = param.samplingsource();
            const string &sourceName = samplingSource.name();
            TexturePtr pTexture;
//            ostringstream debugString;
//            debugString << "setting sampler " << paramName;
            switch (samplingSource.type()) {
                case SamplingSource_Type_CLIP:
                    pTexture.reset(new VolatileTexture(m_Renderer, m_Renderer.getImageDescriptionFromClip(sourceName)));
//                    debugString << " from clip texture";
                    break;
                case SamplingSource_Type_SUPPLIED:
                    pTexture.reset(new DisplayableImage(m_Renderer, sourceName));
//                    debugString << " from supplied texture";
                    break;
                case SamplingSource_Type_SURFACE: {
                    const RenderTargets &targets = m_RenderingContext.renderTargets;
                    auto itr = targets.find(sourceName);
                    if (itr == targets.end())
                        throw runtime_error("unknown render target '" + sourceName + "' to sample from");
                    pTexture = itr->second;
//                    debugString << " from surface";
                    break;
                }
                default:
                    cerr << "SamplingSource with type " << SamplingSource_Type_Name(samplingSource.type()) << " is not supported" << endl;
            }
//            debugString << " '" << sourceName << "'" << endl;
//            cout << debugString.str();
            assert( pTexture );
            m_Renderer.setTexture(m_pShader->getParameter(paramName), param.samplerstate(), pTexture->getTexture());
            m_RenderingContext.textures.push_back(pTexture);
            break;
        }
        default:
            assert(!"not yet implemented");
    }
}

//#define DEBUG_SHADER

CGprogram ShaderFactory::createProgram(const string& code, const string &name) const {
    const char** pProgramOptions = m_Renderer.getShaderOptions(m_Type);
    const CGprofile profile = m_Renderer.getShaderProfile(m_Type);
    const CGprogram cgProgram = cgCreateProgram(m_Renderer.getCgContext(), //context
                                                CG_SOURCE, // compiling source
                                                code.c_str(), // program
                                                profile, // CG profile
                                                "main", //entry point
                                                pProgramOptions //args
            );

    if (!cgIsProgramCompiled(cgProgram)) {
        cerr << "error while compiling the following code" << endl;
        cerr << "----------------------------------------" << endl;
        cerr << code << endl;
        cerr << "----------------------------------------" << endl;
        cerr << "profile : " << cgGetProfileString(profile) << endl;
        cerr << "options : " << endl;
        for (int i = 0; pProgramOptions[i] != '\0'; ++i)
            cerr << pProgramOptions[i] << endl;
    } else {
#ifdef DEBUG_SHADER
        cout << "========================================" << endl;
        cout << "creating shader" << endl;
        cout << "----------------------------------------" << endl;
        cout << cgGetProgramString(cgProgram, CG_PROGRAM_SOURCE) << endl;
        cout << "----------------------------------------" << endl;
        const string compiled = cgGetProgramString(cgProgram, CG_COMPILED_PROGRAM);
        typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
        boost::char_separator<char> sep("\n");
        tokenizer tokens(compiled, sep);
        size_t instructions = 0;
        for (tokenizer::iterator beg = tokens.begin(); beg != tokens.end(); ++beg, ++instructions)
            if (boost::starts_with(*beg, "//"))
                instructions = 0;
        cout << name << " compiled to " << instructions << " instructions" << endl;
        cout << "========================================" << endl;
#endif
    }
    return cgProgram;
}

ShaderBasePtr ShaderFactory::getShader() const {
    return m_pShader;
}
