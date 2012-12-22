/*
 * Shader.cpp
 *
 *  Created on: Nov 28, 2012
 *      Author: Guillaume Chatelet
 */

#include "Shader.hpp"
#include <stdexcept>

Program::Program(const SharedVertexShader &vertexShader, const SharedFragmentShader &fragmentShader) :
		programId(glCreateProgram()), pVertexShader(vertexShader), pFragmentShader(fragmentShader) {
	glAttachShader(programId, pVertexShader->getId());
	glAttachShader(programId, pFragmentShader->getId());
	glLinkProgram(programId);
	checkProgramError(programId);
}

Program::~Program() {
	glDetachShader(programId, pVertexShader->getId());
	glDetachShader(programId, pFragmentShader->getId());
	glDeleteProgram(programId);
}

void Program::use() const {
	glValidateProgram(programId);
	checkError();
	glUseProgram(programId);
}

GLuint Program::getUniformLocation(const char* pUniformName) const {
	const GLuint uniformId = glGetUniformLocation(programId, pUniformName);
	if (uniformId != GLuint(-1))
		return uniformId;
	char error[1024];
	snprintf(error, sizeof(error), "OpenGL : no uniform named '%s'", pUniformName);
	throw std::runtime_error(error);
}

SharedFragmentShader makeFragmentShader(const char* pSource) {
	return std::make_shared<FragmentShader>(pSource);
}

SharedVertexShader makeVertexShader(const char* pSource) {
	return std::make_shared<VertexShader>(pSource);
}

SharedFragmentShader loadFragmentShader(const char* pFilename) {
	std::string shader(slurpFile(pFilename));
	return makeFragmentShader(shader.c_str());
}

SharedVertexShader loadVertexShader(const char* pFilename) {
	std::string shader(slurpFile(pFilename));
	return makeVertexShader(shader.c_str());
}

//std::vector<std::string> Program::getUniformNames() const {
//	std::vector<std::string> names;
//	int total = -1;
//	glGetProgramiv(programId, GL_ACTIVE_UNIFORMS, &total);
//	for (int i = 0; i < total; ++i) {
//		int name_len = -1, num = -1;
//		GLenum type = GL_ZERO;
//		char name[100];
//		glGetActiveUniform(programId, GLuint(i), sizeof(name) - 1, &name_len, &num, &type, name);
//		name[name_len] = 0;
//		names.push_back(name);
//	}
//	return names;
//}
