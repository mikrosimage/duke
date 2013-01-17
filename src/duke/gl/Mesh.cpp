/*
 * Mesh.cpp
 *
 *  Created on: Nov 29, 2012
 *      Author: Guillaume Chatelet
 */
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
	case GL_PATCHES:
		return primitiveType;
	default:
		throw std::runtime_error("invalid primitive mesh type");
	}
}

Mesh::Mesh(GLuint primitiveType, const VertexPosUv0 *pVBegin, const size_t vertexCount) :
		primitiveType(checkType(primitiveType)), vertexCount(vertexCount) {
	const auto vaoBinder = scope_bind(vao);
	const auto vboBinder = scope_bind(vbo);
	vbo.bufferData(vertexCount * sizeof(VertexPosUv0), pVBegin);
	glCheckError();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPosUv0), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPosUv0), (const GLvoid*) (sizeof(glm::vec3)));
	glEnableVertexAttribArray(1);
	glCheckError();
}

Mesh::~Mesh() {
}

void Mesh::draw() const {
	const auto vaoBinder = scope_bind(vao);
	glCheckError();
	callDraw();
	glCheckError();
}

void Mesh::callDraw() const {
	glDrawArrays(primitiveType, 0, vertexCount);
}

IndexedMesh::IndexedMesh(GLuint primitiveType, const VertexPosUv0 *pVBegin, const size_t vertexCount, const GLuint *pIBegin, const size_t indexCount) :
		Mesh(primitiveType, pVBegin, vertexCount), indexCount(indexCount) {
	const auto ivboBinder = scope_bind(ivbo);
	ivbo.bufferData(indexCount * sizeof(GLuint), pIBegin);
	glCheckError();
}

void IndexedMesh::callDraw() const {
	const auto iboBinder = scope_bind(ivbo);
	glDrawElements(primitiveType, indexCount, GL_UNSIGNED_INT, 0);
}

SharedMesh getSquare() {
	using namespace std;
	const float z = 1;
	const vector<VertexPosUv0> vertices = { //
			{ glm::vec3(-1, -1, z), glm::vec2(0, 0) }, //
					{ glm::vec3(-1, 1, z), glm::vec2(0, 1) }, //
					{ glm::vec3(1, 1, z), glm::vec2(1, 1) }, //
					{ glm::vec3(1, -1, z), glm::vec2(1, 0) } };
	return make_shared<Mesh>(GL_TRIANGLE_FAN, vertices.data(), vertices.size());
}
