#include "Shader.hpp"

SharedFragmentShader makeFragmentShader(const char* pSource) {
	return std::make_shared<FragmentShader>(pSource);
}

SharedVertexShader makeVertexShader(const char* pSource) {
	return std::make_shared<VertexShader>(pSource);
}

SharedFragmentShader loadFragmentShaderFile(const char* pFilename) {
	std::string shader(slurpFile(pFilename));
	return makeFragmentShader(shader.c_str());
}

SharedVertexShader loadVertexShaderFile(const char* pFilename) {
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
