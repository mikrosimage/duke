#ifndef MESHBUILDER_H_
#define MESHBUILDER_H_

#include <dukeapi/IMessageIO.h>
#include <player.pb.h>
#include <iostream>

namespace duke {
namespace protocol {

enum MeshShape {
    MS_Cube = 0, MS_Plane = 1
};

static inline void buildVertex(Vertex* const pVertex //
                 , float x //
                 , float y //
                 , float z) {
    const float u = x > 0 ? 1 : 0;
    const float v = y > 0 ? 0 : 1;

    pVertex->add_data(x);
    pVertex->add_data(y);
    pVertex->add_data(z);
    pVertex->add_data(u);
    pVertex->add_data(v);
}

static inline void buildVertex(Vertex * const pVertex //
                 , float x //
                 , float y //
                 , float z //
                 , float u //
                 , float v) {
    pVertex->add_data(x);
    pVertex->add_data(y);
    pVertex->add_data(z);
    pVertex->add_data(u);
    pVertex->add_data(v);
}

static inline void buildCubeVertexBuffer(VertexBuffer* const pVertexBuffer) {
    pVertexBuffer->add_flag(VertexBuffer_VertexType_POS);
    pVertexBuffer->add_flag(VertexBuffer_VertexType_UV1);
    buildVertex(pVertexBuffer->add_vertex(), -1, 1, -1); // 0
    buildVertex(pVertexBuffer->add_vertex(), 1, 1, -1); // 1
    buildVertex(pVertexBuffer->add_vertex(), -1, -1, -1); // 2
    buildVertex(pVertexBuffer->add_vertex(), 1, -1, -1); // 3
    buildVertex(pVertexBuffer->add_vertex(), -1, 1, 1); // 4
    buildVertex(pVertexBuffer->add_vertex(), 1, 1, 1); // 5
    buildVertex(pVertexBuffer->add_vertex(), -1, -1, 1); // 6
    buildVertex(pVertexBuffer->add_vertex(), 1, -1, 1); // 7
}

static inline void buildCubeIndexBuffer(IndexBuffer* const pIndexBuffer) {
    const int indices[] = { 0, 2, 1, 2, 3, 1, // side 1
                    4, 6, 0, 6, 2, 0, // side 2
                    7, 6, 5, 6, 4, 5, // side 3
                    3, 7, 1, 7, 5, 1, // side 4
                    4, 0, 5, 0, 1, 5, // side 5
                    3, 2, 7, 2, 6, 7 // side 6
                    };
    const int indices_size = sizeof(indices) / sizeof(int);

    for (int i = 0; i < indices_size; ++i)
        pIndexBuffer->add_index(indices[i]);
}

static inline void buildPlaneVertexBuffer(VertexBuffer* const pVertexBuffer //
                            , const float x0 //
                            , const float y0 //
                            , const float xsize //
                            , const float ysize) {
    pVertexBuffer->add_flag(::duke::protocol::VertexBuffer_VertexType_POS);
    pVertexBuffer->add_flag(::duke::protocol::VertexBuffer_VertexType_UV1);
    buildVertex(pVertexBuffer->add_vertex(), x0, y0, 1, 0, 1); // 0
    buildVertex(pVertexBuffer->add_vertex(), x0 + xsize, y0, 1, 1, 1); // 1
    buildVertex(pVertexBuffer->add_vertex(), x0, y0 + ysize, 1, 0, 0); // 2
    buildVertex(pVertexBuffer->add_vertex(), x0 + xsize, y0 + ysize, 1, 1, 0); // 3
}

static inline void buildPlaneIndexBuffer(IndexBuffer* const pIndexBuffer) {
    const int indices[] = { 0, 2, 1, 2, 3, 1 };
    const int indices_size = sizeof(indices) / sizeof(int);
    for (int i = 0; i < indices_size; ++i)
        pIndexBuffer->add_index(indices[i]);
}

static inline void buildMesh(Mesh & _m //
               , MeshShape _shape = MS_Cube //
               , std::string _name = "defaultMeshName"//
               , const float _x0 = 0.f //
               , const float _y0 = 0.f //
               , const float _xSize = 1.f //
               , const float _ySize = 1.f) {

    _m.set_name(_name);
    switch (_shape) {
        case MS_Cube:
            _m.set_type(Mesh_MeshType_TRIANGLELIST);
            buildCubeVertexBuffer(_m.mutable_vertexbuffer());
            buildCubeIndexBuffer(_m.mutable_indexbuffer());
            break;
        case MS_Plane:
            _m.set_type(Mesh_MeshType_TRIANGLELIST);
            buildPlaneVertexBuffer(_m.mutable_vertexbuffer(), _x0, _y0, _xSize, _ySize);
            buildPlaneIndexBuffer(_m.mutable_indexbuffer());
            break;
        default:
            assert(!"mesh type not handled");
            std::cerr << "mesh type not handled" << std::endl;
            break;
    }
}

static inline void addMesh(IMessageIO & _queue //
             , MeshShape _shape = MS_Cube //
             , std::string _name = "defaultMeshName"//
             , const float _x0 = 0.f //
             , const float _y0 = 0.f //
             , const float _xSize = 1.f //
             , const float _ySize = 1.f) {

    Mesh m;
    buildMesh(m, _shape, _name, _x0, _y0, _xSize, _ySize);
    push(_queue, m);
}

} /* namespace protocol */
} /* namespace duke */

#endif /* MESHBUILDER_H_ */
