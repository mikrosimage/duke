#ifndef IMESHBASE_H_
#define IMESHBASE_H_

#include "IResource.h"
#include "Buffer.h"
#include "Enums.h"

class IRenderer;

class IMeshBase : public IResource
{
public:
	struct TVertex
	{
		float x, y, z;
		float u, v;
		//		unsigned long color;
	};
	typedef unsigned short TIndex;

	IMeshBase( TPrimitiveType type, Buffer<TVertex> vertexBuffer, Buffer<TIndex> indexBuffer );
	virtual ~IMeshBase();

	void render( IRenderer& ) const;

	virtual const ::resource::Type getResourceType() const { return ::resource::MESH; }
	virtual std::size_t getSize() const { return m_VertexBuffer.size() + m_IndexBuffer.size(); }
private:
	unsigned long getPrimitiveCount() const;
	TPrimitiveType m_PrimitiveType;
	Buffer<TVertex> m_VertexBuffer;
	Buffer<TIndex> m_IndexBuffer;
	unsigned long m_iPrimitiveCount;
};

#endif /* IMESHBASE_H_ */
