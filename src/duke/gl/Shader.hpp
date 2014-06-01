#pragma once

#include "duke/base/NonCopyable.hpp"
#include "duke/gl/GlUtils.hpp"
#include "duke/gl/GL.hpp"

#include <memory>
#include <cstring>
#include <cassert>

template <GLuint SHADER_TYPE>
class Shader : public noncopyable {
 public:
  Shader(const char* pSource) : shaderId(glCreateShader(SHADER_TYPE)) {
    assert(pSource);
    assert(isValidShaderType());
    const int length = strlen(pSource);
    glShaderSource(shaderId, 1, &pSource, &length);
    glCompileShader(shaderId);
    checkShaderError(shaderId, pSource);
  }
  virtual ~Shader() { glDeleteShader(shaderId); }
  virtual GLuint getId() const { return shaderId; }

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

SharedFragmentShader loadFragmentShaderFile(const char* pFilename);
SharedVertexShader loadVertexShaderFile(const char* pFilename);
