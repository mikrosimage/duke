/*
 * Mesh.hpp
 *
 *  Created on: Nov 28, 2012
 *      Author: Guillaume Chatelet
 */

#ifndef MESH_HPP_
#define MESH_HPP_

#include <duke/gl/GlObjects.h>
#include <duke/gl/GLUtils.h>

#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace duke {

struct VertexPosUv0 {
	glm::vec3 position;
	glm::vec2 uv0;
	VertexPosUv0(glm::vec3 pos) :
			position(pos) {
	}
	VertexPosUv0(glm::vec3 pos, glm::vec2 uv0) :
			position(pos), uv0(uv0) {
	}
};

class Mesh: public noncopyable {
public:
	Mesh(GLuint primitiveType, const VertexPosUv0 *pVBegin, const size_t vertexCount);
	virtual ~Mesh();

	void draw() const;
protected:
	const GLuint primitiveType;
	virtual void callDraw() const;
private:
	const size_t vertexCount;
	const duke::gl::GlVertexArrayObject vao;
	const duke::gl::GlStaticVbo vbo;
};

class IndexedMesh: public Mesh {
public:
	IndexedMesh(GLuint primitiveType, const VertexPosUv0 *pVBegin, const size_t vertexCount, const GLuint *pIBegin, const size_t indexCount);
protected:
	virtual void callDraw() const;
private:
	const size_t indexCount;
	const duke::gl::GlStaticIndexedVbo ivbo;
};

typedef std::shared_ptr<Mesh> SharedMesh;
typedef std::shared_ptr<IndexedMesh> SharedIndexedMesh;

SharedMesh createSquare();
SharedMesh createLine();

}  // namespace duke

#endif /* MESH_HPP_ */
