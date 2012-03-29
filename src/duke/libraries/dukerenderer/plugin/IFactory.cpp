#include "IFactory.h"
#include <iostream>

void cgErrorCallback() {
    CGerror err = cgGetError();

    switch (err) {
        case CG_COMPILER_ERROR:
            std::cerr << "Cg compiler error : " << cgGetErrorString(err) << std::endl;
            break;
        case CG_NO_ERROR:
            return;
        default:
            std::cerr << "Cg error : " << cgGetErrorString(err) << std::endl;
            break;
    }
    exit(1);
}

namespace {

const char* EMPTY[] = { NULL, NULL };
const char* float4 = "float4";
const char* float2 = "float2";

} // namespace

static duke::protocol::FunctionPrototype build(const char* name, const char* body) {
    duke::protocol::FunctionPrototype f;
    f.set_body(body);
    duke::protocol::FunctionSignature *pS = f.mutable_signature();
    pS->set_name(name);
    pS->set_returntype(float4);
    pS->add_operandtype(float4);
    return f;
}

void IFactory::add(const duke::protocol::FunctionPrototype &function) {
    if (!m_PrototypeFactory.hasPrototype(function.signature().name()))
        m_PrototypeFactory.setPrototype(function);
}

IFactory::IFactory() :
        m_VSProfile(CG_PROFILE_UNKNOWN), m_PSProfile(CG_PROFILE_UNKNOWN), m_Context(cgCreateContext()) {
    //	cgSetErrorCallback(cgErrorCallback);
    m_VSOptions = EMPTY;
    m_PSOptions = EMPTY;
    // adding default shaders if not set
    add(build("cineontolin", "return 1.010915615730753*(pow(float4(10), (1023*@0-685)/300)-0.010797751623277);"));
    add(build("srgbtolin", "return @0 <= 0.04045 ? @0/12.92 : pow((@0+0.055)/1.055,2.4);"));
    add(build("rgbatobgra", "return @0.bgra;"));
    add(build("lintosrgb", "return saturate(@0 <= 0.0031308 ? 12.92*@0 : (1.055*pow(@0,1/2.4))-0.055);"));
    add(build("tenbitunpackfloat",
              "int4 copy = @0*255;@0.r = (copy.r*4) + (copy.g/64);@0.g = ((copy.g%64)*16) + (copy.b/16);@0.b = ((copy.b%16)*64) + (copy.a/4);return @0/1023;"));
    add(build("lintocineon", "return 4.2453028534042214e-4*(300*log(0.98920224837672*@0+0.010797751623277)+1577.270788700921);"));
    add(build("expand_cgrtosmpte", "return @0 * ((940 - 64) / 1023.) + (64 / 1023.);"));
    add(build("contract_smptetocgr", "return (@0 - (64 / 1023.)) / ((940 - 64) / 1023.);"));
    duke::protocol::FunctionPrototype sampler = build("sample2d", "return tex2D( $0, @0 );");
    sampler.mutable_signature()->set_operandtype(0, float2);
    sampler.mutable_signature()->add_parametertype("sampler2D");
    add(sampler);

    try {
        const char * pFolder = "shader_prototypes";
        m_PrototypeFactory.loadFromFolder(pFolder);
        std::cout << "[Shader] loading additional shaders from " << pFolder << std::endl;
    } catch (std::exception &e) {
        std::cout << "[Shader] using default shaders" << std::endl;
    }
}

IFactory::~IFactory() {
    if (m_Context)
        cgDestroyContext(m_Context);
}

CGcontext IFactory::getCgContext() const {
    return m_Context;
}

ResourceManager& IFactory::getResourceManager() {
    return m_ResourceManager;
}

TexturePool& IFactory::getTexturePool() {
    return m_TexturePool;
}

bool IFactory::hasCapability(TCapability capability) const {
    const TCapabilityMap::const_iterator entry = m_Capabilities.find(capability);

    assert( entry != m_Capabilities.end());
    return entry->second;
}

CGprofile IFactory::getShaderProfile(TShaderType Type) const {
    return Type == SHADER_VERTEX ? m_VSProfile : m_PSProfile;
}

const char* * IFactory::getShaderOptions(TShaderType Type) const {
    return Type == SHADER_VERTEX ? m_VSOptions : m_PSOptions;
}

PrototypeFactory& IFactory::getPrototypeFactory() {
    return m_PrototypeFactory;
}
