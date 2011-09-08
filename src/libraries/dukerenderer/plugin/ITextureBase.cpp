#include "ITextureBase.h"
#include "utils/PixelUtils.h"

ITextureBase::ITextureBase( const ImageDescription& description, const TPixelFormat texturePixelFormat, long usageFlag )
	: IResource( "texture" ),
	m_Description( description ),
	m_TexturePixelFormat( texturePixelFormat ),
	m_iUsageFlag( usageFlag ) {}

ITextureBase::~ITextureBase() {}

bool ITextureBase::isMipMap() const
{
	return ( m_iUsageFlag & TEX_MIPMAP ) > 0;
}

bool ITextureBase::isAutoMipMap() const
{
	return ( m_iUsageFlag & TEX_AUTOMIPMAP ) > 0;
}

bool ITextureBase::isRenderTarget() const
{
	return ( m_iUsageFlag & TEX_RENTERTARGET ) > 0;
}

int ITextureBase::getWidth() const
{
	return m_Description.width;
}

int ITextureBase::getHeight() const
{
	return m_Description.height;
}

int ITextureBase::getDepth() const
{
	return m_Description.depth;
}

TPixelFormat ITextureBase::getFormat() const
{
	return m_TexturePixelFormat;
}

