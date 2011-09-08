/*
 * IShaderBase.h
 *
 *  Created on: 11 mai 2010
 *      Author: Guillaume Chatelet
 */

#include "IShaderBase.h"
#include <iostream>
#include <stdexcept>

using namespace std;

IShaderBase::IShaderBase(CGprogram program, TShaderType type) :
    IResource("shader"), m_Program(program), m_Type(type) {
    CGparameter param = cgGetFirstParameter(m_Program, CG_GLOBAL);
    while (param) {
        appendParameter(param);
        param = cgGetNextParameter(param);
    }
}

void IShaderBase::appendParameter(const CGparameter param) {
    if (param == NULL)
        return;
    if (cgGetParameterType(param) == CG_STRUCT)
        return;
    const string name = cgGetParameterName(param);
    if (name == "main")
        return;
//    cerr << cgGetTypeString(cgGetParameterType(param)) << " " << name << endl;
    m_ParameterNames.push_back(name);
}

IShaderBase::~IShaderBase() {
    if (m_Program)
        cgDestroyProgram(m_Program);
}

void IShaderBase::setParameter(const std::string& name, const float* value, const int size, const bool logIfInexistent) const {
    const CGparameter parameter = getParameter(name);

    if (parameter == NULL) {
        if (logIfInexistent)
            std::cerr << "no parameter named " << name << std::endl;
        return;
    }
    setParameter(parameter, value, size);
}

CGparameter IShaderBase::getParameter(const std::string& name) const {
    return cgGetNamedParameter(m_Program, name.c_str());
}

const vector<string>& IShaderBase::getParameterNames() const {
    return m_ParameterNames;
}

CGprogram IShaderBase::getProgram() const {
    return m_Program;
}

TShaderType IShaderBase::getType() const {
    return m_Type;
}

