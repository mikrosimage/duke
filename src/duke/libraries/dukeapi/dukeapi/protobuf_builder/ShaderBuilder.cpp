/*
 * ShaderBuilder.cpp
 *
 *  Created on: 24 fevr. 2012
 *      Author: Guillaume Chatelet
 */

#include "ShaderBuilder.h"
#include <dukeapi/IMessageIO.h>
#include <dukeapi/messageBuilder/ShaderBuilder.h>
#include <dukeapi/messageBuilder/GradingBuilder.h>
#include <dukeapi/messageBuilder/MeshBuilder.h>

namespace duke {
namespace protocol {
namespace shader_builder {

static inline StaticParameter buildStaticSamplerParam(std::string _name //
                , std::string _srcName //
                , SamplingSource_Type _type = SamplingSource_Type_CLIP //
                , std::string _scopeName = "") {
    StaticParameter parameter;
    // parameters nomenclature: clip_name | param_name
    if (_srcName.empty())
        throw std::runtime_error("Error building StaticParameter: Sampler: invalid source name.");

    if (_scopeName.empty() && _type == SamplingSource_Type_CLIP)
        _scopeName = _srcName;

    if (!_scopeName.empty())
        _name = _scopeName + "|" + _name;

    parameter.set_name(_name);
    parameter.set_type(StaticParameter_Type_SAMPLER);
    // ...sampling source
    SamplingSource* pSamplingSource = parameter.mutable_samplingsource();
    pSamplingSource->set_type(_type);
    pSamplingSource->set_name(_srcName);
    // ...sampler state
    SamplerState* pSamplerState = NULL;
    pSamplerState = parameter.add_samplerstate();
    pSamplerState->set_type(SamplerState_Type_MIN_FILTER);
    pSamplerState->set_value(SamplerState_Value_TEXF_POINT);
    pSamplerState = parameter.add_samplerstate();
    pSamplerState->set_type(SamplerState_Type_MAG_FILTER);
    pSamplerState->set_value(SamplerState_Value_TEXF_POINT);
    pSamplerState = parameter.add_samplerstate();
    pSamplerState->set_type(SamplerState_Type_WRAP_S);
    pSamplerState->set_value(SamplerState_Value_WRAP_BORDER);
    pSamplerState = parameter.add_samplerstate();
    pSamplerState->set_type(SamplerState_Type_WRAP_T);
    pSamplerState->set_value(SamplerState_Value_WRAP_BORDER);
    return parameter;
}

static inline AutomaticParameter automaticClipSourceParam(std::string name, std::string clipName) {
    AutomaticParameter parameter;
    if (clipName.empty())
        throw std::runtime_error("Error building AutomaticParameter: invalid clip name.");

    name = clipName + "|" + name;
    parameter.set_name(name);
    parameter.set_type(AutomaticParameter_Type_FLOAT3_TEX_DIM);

    SamplingSource *pSource = parameter.mutable_samplingsource();
    pSource->set_type(SamplingSource_Type_CLIP);
    pSource->set_name(clipName);
    return parameter;
}

static inline AutomaticParameter automaticParameter(const char * name) {
    AutomaticParameter parameter;
    parameter.set_name(name);
    parameter.set_type(AutomaticParameter_Type_FLOAT3_TEX_DIM);
    return parameter;
}

static inline StaticParameter staticParameter(const char * name, float value) {
    StaticParameter parameter;
    parameter.set_name(name);
    parameter.set_type(StaticParameter_Type_FLOAT);
    parameter.add_floatvalue(value);
    return parameter;
}

static inline Mesh plane() {
    Mesh mesh;
    mesh.set_name("plane");
    mesh.set_type(Mesh_MeshType_TRIANGLELIST);
    buildPlaneVertexBuffer(mesh.mutable_vertexbuffer(), -1, -1, 2, 2);
    buildPlaneIndexBuffer(mesh.mutable_indexbuffer());
    return mesh;
}

void pushCommonMessages(IOQueueInserter& inserter) {
    inserter = automaticParameter(DISPLAY_DIM);
    inserter = staticParameter(DISPLAY_MODE, 0);
    inserter = staticParameter(IMAGE_RATIO, 0);
    inserter = staticParameter(ZOOM, 1);
    inserter = staticParameter(PANX, 0);
    inserter = staticParameter(PANY, 0);
    inserter = staticParameter(PANY, 0);
    inserter = plane();
}

void adapt(IOQueueInserter& inserter, Clip &clip, const std::string& extension) {
    Shader vertexShader;
    buildVertexShader(vertexShader, "vs", fittableTransformVs, clip.name());
    vertexShader.add_parametername(DISPLAY_DIM);
    vertexShader.add_parametername(IMAGE_DIM);
    vertexShader.add_parametername(DISPLAY_MODE);
    vertexShader.add_parametername(IMAGE_RATIO);
    vertexShader.add_parametername(ZOOM);
    vertexShader.add_parametername(PANX);
    vertexShader.add_parametername(PANY);

    Shader pixelShader;
    buildPixelShader(pixelShader, "ps", NULL, clip.name());
    addShadingNode(pixelShader, "rgbatobgra", 1);
    if (extension == ".dpx")
        addShadingNode(pixelShader, "tenbitunpackfloat", 2);

    Grading &grading = *clip.mutable_grade();
    addEffectToGrading(grading, pixelShader.name(), vertexShader.name(), "plane", true);

    inserter = vertexShader;
    inserter = pixelShader;
    inserter = automaticClipSourceParam(IMAGE_DIM, clip.name());
    inserter = buildStaticSamplerParam("sampler", clip.name(), SamplingSource_Type_CLIP);
}

} // namespace shader_builder
} /* namespace protocol */
} /* namespace duke */
