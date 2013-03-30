#include "Mesh.hpp"

#include <stdexcept>

static inline GLuint checkType(GLuint primitiveType) {
	switch (primitiveType) {
	case GL_POINTS:
	case GL_LINES:
	case GL_LINE_LOOP:
	case GL_LINE_STRIP:
	case GL_LINE_STRIP_ADJACENCY:
	case GL_LINES_ADJACENCY:
	case GL_TRIANGLES:
	case GL_TRIANGLE_STRIP:
	case GL_TRIANGLE_FAN:
	case GL_TRIANGLE_STRIP_ADJACENCY:
	case GL_TRIANGLES_ADJACENCY:
		//case GL_PATCHES:
		return primitiveType;
	default:
		throw std::runtime_error("invalid primitive mesh type");
	}
}

namespace duke {

Mesh::Mesh(GLuint primitiveType, const VertexPosUv0 *pVBegin, const size_t vertexCount) :
		primitiveType(checkType(primitiveType)), vertexCount(vertexCount), vao(), vbo() {
	auto vaoBound = vao.scope_bind();
	auto vboBound = vbo.scope_bind_buffer();
	glBufferData(vbo.target, vertexCount * sizeof(VertexPosUv0), pVBegin, vbo.usage);
	glCheckError();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPosUv0), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPosUv0), (const GLvoid*) (sizeof(glm::vec3)));
	glEnableVertexAttribArray(1);
	glCheckError();
}

Mesh::~Mesh() {
}

void Mesh::bind() const {
	vao.bind();
}

void Mesh::draw() const {
	callDraw();
	glCheckError();
}

void Mesh::unbind() const {
	vao.unbind();
}

void Mesh::callDraw() const {
	auto bound = scope_bind();
	glDrawArrays(primitiveType, 0, vertexCount);
}

IndexedMesh::IndexedMesh(GLuint primitiveType, const VertexPosUv0 *pVBegin, const size_t vertexCount, const GLuint *pIBegin, const size_t indexCount) :
		Mesh(primitiveType, pVBegin, vertexCount), indexCount(indexCount), ivbo() {
	auto ivboBound = ivbo.scope_bind_buffer();
	glBufferData(ivbo.target, indexCount * sizeof(GLuint), pIBegin, ivbo.usage);
	glCheckError();
}

void IndexedMesh::bind() const {
	Mesh::bind();
	ivbo.bind();
}

void IndexedMesh::callDraw() const {
	auto bound = scope_bind();
	glDrawElements(primitiveType, indexCount, GL_UNSIGNED_INT, 0);
}

void IndexedMesh::unbind() const {
	ivbo.unbind();
	Mesh::unbind();
}

SharedMesh createSquare() {
	using namespace std;
	const float z = 1;
	const vector<VertexPosUv0> vertices = { //
			{ glm::vec3(-1, -1, z), glm::vec2(0, 0) }, //
					{ glm::vec3(-1, 1, z), glm::vec2(0, 1) }, //
					{ glm::vec3(1, 1, z), glm::vec2(1, 1) }, //
					{ glm::vec3(1, -1, z), glm::vec2(1, 0) } };
	return make_shared<Mesh>(GL_TRIANGLE_FAN, vertices.data(), vertices.size());
}

SharedMesh createLine() {
	using namespace std;
	const float z = 1;
	const vector<VertexPosUv0> vertices = { //
			{ glm::vec3(-1, -1, z), glm::vec2(0, 0) }, //
					{ glm::vec3(1, 1, z), glm::vec2(1, 1) } };
	return make_shared<Mesh>(GL_LINES, vertices.data(), vertices.size());
}
}  // namespace duke
