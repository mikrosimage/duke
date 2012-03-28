/*
 * MeshBuilder.cpp
 *
 *  Created on: 22 mars 2012
 *      Author: Guillaume Chatelet
 */

#include "MeshBuilder.h"
#include <boost/make_shared.hpp>

namespace duke {
namespace protocol {

const std::string MeshBuilder::plane("plane");

static void buildVertex(Vertex * const pVertex //
                , float x, float y, float z //
                , float u, float v) {
    pVertex->add_data(x);
    pVertex->add_data(y);
    pVertex->add_data(z);
    pVertex->add_data(u);
    pVertex->add_data(v);
}

static inline void buildPlaneVertexBuffer(VertexBuffer* const pVertexBuffer //
                , const float x, const float y //
                , const float width, const float height) {
    pVertexBuffer->add_flag(VertexBuffer_VertexType_POS);
    pVertexBuffer->add_flag(VertexBuffer_VertexType_UV1);
    buildVertex(pVertexBuffer->add_vertex(), x, y, 1, 0, 1); // 0
    buildVertex(pVertexBuffer->add_vertex(), x + width, y, 1, 1, 1); // 1
    buildVertex(pVertexBuffer->add_vertex(), x, y + height, 1, 0, 0); // 2
    buildVertex(pVertexBuffer->add_vertex(), x + width, y + height, 1, 1, 0); // 3
}

static inline void buildPlaneIndexBuffer(IndexBuffer* const pIndexBuffer) {
    const int indices[] = { 0, 2, 1, 2, 3, 1 };
    const int indices_size = sizeof(indices) / sizeof(int);
    for (int i = 0; i < indices_size; ++i)
        pIndexBuffer->add_index(indices[i]);
}

Mesh MeshBuilder::buildPlane(const std::string &name) {
    Mesh mesh;
    mesh.set_name(name);
    mesh.set_type(Mesh_MeshType_TRIANGLELIST);
    buildPlaneVertexBuffer(mesh.mutable_vertexbuffer(), -1, -1, 2, 2);
    buildPlaneIndexBuffer(mesh.mutable_indexbuffer());
    return mesh;
}

boost::shared_ptr<google::protobuf::Message> MeshBuilder::build(const std::string name) {
    if (name != MeshBuilder::plane)
        return boost::shared_ptr<Mesh>();
    return boost::make_shared<Mesh>(buildPlane(MeshBuilder::plane));
}

} // namespace protocol
} // namespace duke
