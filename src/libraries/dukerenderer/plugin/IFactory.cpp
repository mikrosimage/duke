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

const char* EMPTY[] = { NULL, NULL };

IFactory::IFactory() :
    m_VSProfile(CG_PROFILE_UNKNOWN), m_PSProfile(CG_PROFILE_UNKNOWN), m_Context(cgCreateContext()),
            m_PrototypeFactory("shader_prototypes")

{
    //	cgSetErrorCallback(cgErrorCallback);
    m_VSOptions = EMPTY;
    m_PSOptions = EMPTY;
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

    assert( entry != m_Capabilities.end() );
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
