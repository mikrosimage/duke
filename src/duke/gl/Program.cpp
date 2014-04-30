#include "Program.hpp"
#include <stdexcept>

namespace duke {

Program::Program(const SharedVertexShader& vertexShader, const SharedFragmentShader& fragmentShader)
    : programId(glCreateProgram()), pVertexShader(vertexShader), pFragmentShader(fragmentShader) {
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
#ifndef NDEBUG
  glValidateProgram(programId);
  glCheckError();
#endif
  glUseProgram(programId);
  glCheckError();
}

GLint Program::getUniformLocation(const char* pUniformName) const {
  const GLint uniformId = glGetUniformLocation(programId, pUniformName);
  if (uniformId != -1) return uniformId;
  char error[1024];
  snprintf(error, sizeof(error), "OpenGL : no uniform named '%s'", pUniformName);
  throw std::runtime_error(error);
}

Program::CacheEntry& Program::getOrCreate(const char* pUniformName) {
  auto pFound = m_Cache.find(pUniformName);
  if (pFound == m_Cache.end())
    pFound = m_Cache.insert(std::make_pair(pUniformName, CacheEntry{getUniformLocation(pUniformName)})).first;
  return pFound->second;
}

namespace {

template <typename T>
bool update(char* cached, const T& newValue) {
  if (memcmp(cached, &newValue, sizeof(newValue)) != 0) {
    memcpy(cached, &newValue, sizeof(newValue));
    return true;
  }
  return false;
}

}  // namespace

void Program::glUniform1f(const char* pUniformName, GLfloat v0) {
  CacheEntry& entry = getOrCreate(pUniformName);
  const GLfloat data[] = {v0};
  if (update(entry.data, data)) ::glUniform1f(entry.location, v0);
}
void Program::glUniform2f(const char* pUniformName, GLfloat v0, GLfloat v1) {
  CacheEntry& entry = getOrCreate(pUniformName);
  const GLfloat data[] = {v0, v1};
  if (update(entry.data, data)) ::glUniform2f(entry.location, v0, v1);
}
void Program::glUniform3f(const char* pUniformName, GLfloat v0, GLfloat v1, GLfloat v2) {
  CacheEntry& entry = getOrCreate(pUniformName);
  const GLfloat data[] = {v0, v1, v2};
  if (update(entry.data, data)) ::glUniform3f(entry.location, v0, v1, v2);
}
void Program::glUniform4f(const char* pUniformName, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) {
  CacheEntry& entry = getOrCreate(pUniformName);
  const GLfloat data[] = {v0, v1, v2, v3};
  if (update(entry.data, data)) ::glUniform4f(entry.location, v0, v1, v2, v3);
}

void Program::glUniform1i(const char* pUniformName, GLint v0) {
  CacheEntry& entry = getOrCreate(pUniformName);
  const GLint data[] = {v0};
  if (update(entry.data, data)) ::glUniform1i(entry.location, v0);
}
void Program::glUniform2i(const char* pUniformName, GLint v0, GLint v1) {
  CacheEntry& entry = getOrCreate(pUniformName);
  const GLint data[] = {v0, v1};
  if (update(entry.data, data)) ::glUniform2i(entry.location, v0, v1);
}
void Program::glUniform3i(const char* pUniformName, GLint v0, GLint v1, GLint v2) {
  CacheEntry& entry = getOrCreate(pUniformName);
  const GLint data[] = {v0, v1, v2};
  if (update(entry.data, data)) ::glUniform3i(entry.location, v0, v1, v2);
}
void Program::glUniform4i(const char* pUniformName, GLint v0, GLint v1, GLint v2, GLint v3) {
  CacheEntry& entry = getOrCreate(pUniformName);
  const GLint data[] = {v0, v1, v2, v3};
  if (update(entry.data, data)) ::glUniform4i(entry.location, v0, v1, v2, v3);
}

void Program::glUniform1ui(const char* pUniformName, GLuint v0) {
  CacheEntry& entry = getOrCreate(pUniformName);
  const GLuint data[] = {v0};
  if (update(entry.data, data)) ::glUniform1ui(entry.location, v0);
}
void Program::glUniform2ui(const char* pUniformName, GLuint v0, GLuint v1) {
  CacheEntry& entry = getOrCreate(pUniformName);
  const GLuint data[] = {v0, v1};
  if (update(entry.data, data)) ::glUniform2ui(entry.location, v0, v1);
}
void Program::glUniform3ui(const char* pUniformName, GLuint v0, GLuint v1, GLuint v2) {
  CacheEntry& entry = getOrCreate(pUniformName);
  const GLuint data[] = {v0, v1, v2};
  if (update(entry.data, data)) ::glUniform3ui(entry.location, v0, v1, v2);
}
void Program::glUniform4ui(const char* pUniformName, GLuint v0, GLuint v1, GLuint v2, GLuint v3) {
  CacheEntry& entry = getOrCreate(pUniformName);
  const GLuint data[] = {v0, v1, v2, v3};
  if (update(entry.data, data)) ::glUniform4ui(entry.location, v0, v1, v2, v3);
}

} /* namespace duke */
