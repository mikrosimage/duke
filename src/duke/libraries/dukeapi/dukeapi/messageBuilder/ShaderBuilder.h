#ifndef SHADERBUILDER_OLD_H_
#define SHADERBUILDER_OLD_H_

#include "SimpleShaders.h"
#include <player.pb.h>
#include <iostream>

namespace duke {
namespace protocol {

static inline void buildVertexShader(Shader & _vs //
                , std::string _name //
                , const char* _code = NULL //
                , std::string _scopeName = "") {
    if (_scopeName != "")
        _name += "_" + _scopeName;

    _vs.set_name(_name);
    if (_code != NULL)
        _vs.set_code(_code);
}

static inline void buildPixelShader(Shader & _ps //
                , std::string _name //
                , const char* _code = NULL //
                , std::string _scopeName = "") {
    if (_scopeName != "")
        _name += "_" + _scopeName;

    _ps.set_name(_name);
    if (_code != NULL)
        _ps.set_code(_code);
    else {
        Program * p = _ps.mutable_program();
        // main
        FunctionInstance * f = p->add_function();
        FunctionInstance_CallGraph * g = f->mutable_callgraph();
        FunctionSignature * sign = g->mutable_signature();
        sign->set_name("main");
        sign->set_returntype("float4");
        sign->set_varyingouputsemantic("COLOR");
        sign->add_operandtype("PixelInput");
        sign->add_prependdeclaration("struct PixelInput{float2 uv0: TEXCOORD0;};");

        //
        FunctionInstance_CallGraph_FunctionCall * call0 = g->add_call();
        FunctionInstance * f0 = call0->mutable_function();
        f0->set_inlinedvalue("@0.uv0");

        //
        FunctionInstance_CallGraph_FunctionCall * call1 = g->add_call();
        FunctionInstance * f1 = call1->mutable_function();
        f1->set_useprototypenamed("sample2d");
        f1->add_parametername("sampler");
        call1->add_operand(0);
    }
}

static inline void addShadingNode(Shader & _ps //
                , std::string _prototypename //
                , size_t _operand //
                , size_t _funcID = 0) {

    // retrieve CallGraph...
    Program * p = _ps.mutable_program();
    FunctionInstance * f = p->mutable_function(_funcID);
    FunctionInstance_CallGraph * g = f->mutable_callgraph();

    // .. and add a call
    FunctionInstance_CallGraph_FunctionCall * call0 = g->add_call();
    FunctionInstance * f0 = call0->mutable_function();
    f0->set_useprototypenamed(_prototypename);
    call0->add_operand(_operand);
}

} /* namespace protocol */
} /* namespace duke */

#endif /* SHADERBUILDER_OLD_H_ */
