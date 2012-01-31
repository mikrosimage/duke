#include "IMeshBase.h"
#include "IRenderer.h"

IMeshBase::IMeshBase( TPrimitiveType type, Buffer<TVertex> vertexBuffer, Buffer<TIndex> indexBuffer )
	: IResource( "mesh" ),
	m_PrimitiveType( type ),
	m_VertexBuffer( vertexBuffer ),
	m_IndexBuffer( indexBuffer ),
	m_iPrimitiveCount( getPrimitiveCount() ) {}

IMeshBase::~IMeshBase() {}

void IMeshBase::render( IRenderer& renderer ) const
{
	renderer.setVertexBuffer( 0, m_VertexBuffer.getBuffer(), sizeof( TVertex ) );
	if( m_IndexBuffer.size() != 0 )
	{
		renderer.setIndexBuffer( m_IndexBuffer.getBuffer() );
		renderer.drawIndexedPrimitives( m_PrimitiveType, m_iPrimitiveCount );
	}
	else
	{
		renderer.drawPrimitives( m_PrimitiveType, m_iPrimitiveCount );
	}
}

unsigned long IMeshBase::getPrimitiveCount() const
{
	const unsigned long vbSize = m_IndexBuffer.size() != 0 ? m_IndexBuffer.size() : m_VertexBuffer.size();

	switch( m_PrimitiveType )
	{
		case PT_POINTLIST:
			return vbSize;
		case PT_LINELIST:
			return vbSize / 2;
		case PT_LINESTRIP:
			return vbSize - 1;
		case PT_TRIANGLELIST:
			return vbSize / 3;
		case PT_TRIANGLESTRIP:
			return vbSize - 2;
		case PT_TRIANGLEFAN:
			return vbSize - 2;
		default:
			throw std::runtime_error( "invalid primitive" );
	}
}

