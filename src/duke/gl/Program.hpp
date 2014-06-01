#pragma once

#include "duke/gl/Shader.hpp"
#include <map>

namespace duke {

struct Program : public noncopyable {
  Program(const SharedVertexShader& vertexShader, const SharedFragmentShader& fragmentShader);
  ~Program();

  void use() const;
  GLint getUniformLocation(const char* pUniformName) const;

  void glUniform1f(const char* pUniformName, GLfloat v0);
  void glUniform2f(const char* pUniformName, GLfloat v0, GLfloat v1);
  void glUniform3f(const char* pUniformName, GLfloat v0, GLfloat v1, GLfloat v2);
  void glUniform4f(const char* pUniformName, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);

  void glUniform1i(const char* pUniformName, GLint v0);
  void glUniform2i(const char* pUniformName, GLint v0, GLint v1);
  void glUniform3i(const char* pUniformName, GLint v0, GLint v1, GLint v2);
  void glUniform4i(const char* pUniformName, GLint v0, GLint v1, GLint v2, GLint v3);

  void glUniform1ui(const char* pUniformName, GLuint v0);
  void glUniform2ui(const char* pUniformName, GLuint v0, GLuint v1);
  void glUniform3ui(const char* pUniformName, GLuint v0, GLuint v1, GLuint v2);
  void glUniform4ui(const char* pUniformName, GLuint v0, GLuint v1, GLuint v2, GLuint v3);

 private:
  const GLuint programId;
  const SharedVertexShader pVertexShader;
  const SharedFragmentShader pFragmentShader;
  enum {
    DATA_SIZE = 4 * sizeof(GLfloat)
  };
  struct CacheEntry {
    GLint location;
    char data[DATA_SIZE];
  };
  typedef std::map<const char*, CacheEntry> Cache;
  Cache m_Cache;
  CacheEntry& getOrCreate(const char* pUniformName);
};

typedef std::shared_ptr<Program> SharedProgram;

} /* namespace duke */
