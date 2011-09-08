/*
 * Mesh.cpp
 *
 *  Created on: 11 mai 2010
 *      Author: Guillaume Chatelet
 */

#include "Mesh.h"
#include "IRenderer.h"
#include "IMeshBase.h"
#include <iostream>
#include <stdexcept>

Mesh::Mesh( IFactory& factory, const ::protocol::duke::Mesh& mesh )
{
	const std::string name( mesh.name() );

	m_pMeshBase = factory.getResourceManager().get< IMeshBase>( ::resource::MESH, name );

	if( !m_pMeshBase )
	{
		const ::protocol::duke::VertexBuffer& vb = mesh.vertexbuffer();
		std::vector<IMeshBase::TVertex> vertices;
		for( int vertexIndex = 0; vertexIndex < vb.vertex_size(); vertexIndex++ )
		{
			const ::protocol::duke::Vertex& vertex = vb.vertex( vertexIndex );
			IMeshBase::TVertex vertexStruct;
			vertexStruct.x = vertex.data( 0 );
			vertexStruct.y = vertex.data( 1 );
			vertexStruct.z = vertex.data( 2 );
			vertexStruct.u = vertex.data( 3 );
			vertexStruct.v = vertex.data( 4 );
			vertices.push_back( vertexStruct );
		}
		Buffer<IMeshBase::TVertex> vertexBuffer( factory.createVertexBuffer( vertices.size(), BUF_STATIC, vertices.data() ) );
		if( mesh.has_indexbuffer() )
		{
			const ::protocol::duke::IndexBuffer& ib = mesh.indexbuffer();
			const int indicesCount             = ib.index_size();
			std::vector<IMeshBase::TIndex> indices;
			indices.reserve( indicesCount );
			for( int i = 0; i < indicesCount; i++ )
				indices.push_back( ib.index( i ) );
			Buffer<IMeshBase::TIndex> indexBuffer( factory.createIndexBuffer( indicesCount, BUF_STATIC, indices.data() ) );
			m_pMeshBase.reset( new IMeshBase( Enums::Get( mesh.type() ), vertexBuffer, indexBuffer ) );
		}
		else
			m_pMeshBase.reset( new IMeshBase( Enums::Get( mesh.type() ), vertexBuffer, NULL ) );

		factory.getResourceManager().add( name, m_pMeshBase );
	}
}

Mesh::~Mesh() {}

void Mesh::render( IRenderer& renderer ) const
{
	if( m_pMeshBase )
		m_pMeshBase->render( renderer );
}

