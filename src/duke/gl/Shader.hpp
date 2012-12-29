/*
 * Shader.hpp
 *
 *  Created on: Nov 28, 2012
 *      Author: Guillaume Chatelet
 */

#ifndef SHADER_HPP_
#define SHADER_HPP_

#include <duke/NonCopyable.h>
#include <duke/gl/GLUtils.h>

#include <GL/gl.h>

#include <memory>
#include <cstring>
#include <cassert>

template<GLuint SHADER_TYPE>
class Shader: public NonCopyable {
public:
	Shader(const char* pSource) :
			shaderId(glCreateShader(SHADER_TYPE)) {
		assert(pSource);
		assert(isValidShaderType());
		const int length = strlen(pSource);
		glShaderSource(shaderId, 1, &pSource, &length);
		glCompileShader(shaderId);
		checkShaderError(shaderId, pSource);
	}
	virtual ~Shader() {
		glDeleteShader(shaderId);
	}
	virtual GLuint getId() const {
		return shaderId;
	}

private:
	const GLuint shaderId;
	static bool constexpr isValidShaderType() {
		return SHADER_TYPE == GL_VERTEX_SHADER || SHADER_TYPE == GL_FRAGMENT_SHADER;
	}
};

typedef Shader<GL_FRAGMENT_SHADER> FragmentShader;
typedef Shader<GL_VERTEX_SHADER> VertexShader;
typedef std::shared_ptr<FragmentShader> SharedFragmentShader;
typedef std::shared_ptr<VertexShader> SharedVertexShader;

SharedFragmentShader makeFragmentShader(const char* pSource);
SharedVertexShader makeVertexShader(const char* pSource);

SharedFragmentShader loadFragmentShader(const char* pFilename);
SharedVertexShader loadVertexShader(const char* pFilename);

#include "Texture.h"

struct Program: public NonCopyable {
	Program(const SharedVertexShader &vertexShader, const SharedFragmentShader &fragmentShader);
	~Program();
	void use() const;
	GLuint getUniformLocation(const char* pUniformName) const;
//	std::vector<std::string> getUniformNames() const ;
private:
	const GLuint programId;
	const SharedVertexShader pVertexShader;
	const SharedFragmentShader pFragmentShader;
};

typedef std::shared_ptr<Program> SharedProgram;

#endif /* SHADER_HPP_ */
