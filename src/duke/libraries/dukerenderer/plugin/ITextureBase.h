#ifndef TEXTUREBASE_H_
#define TEXTUREBASE_H_

#include "Enums.h"
#include "math/Vector2.h"
#include "math/Rect.h"
#include "IResource.h"
#include "Image.h"

struct ImageSpec;

class ITextureBase : public IResource
{
public:
	virtual ~ITextureBase();

	bool         isMipMap() const;
	bool         isAutoMipMap() const;
	bool         isRenderTarget() const;
	int          getWidth() const;
	int          getHeight() const;
	int          getDepth() const;
	TPixelFormat getFormat() const;

	virtual const ::resource::Type getResourceType() const { return ::resource::TEXTURE; }
	virtual std::size_t getSize() const { return sizeof(ITextureBase); }

protected:
	friend class VolatileTexture;
	friend class DisplayableImage;
	ITextureBase( const ImageDescription& description, const TPixelFormat texturePixelFormat, long usageFlag );
	virtual void update( const ImageDescription& description, const unsigned char* pData ) = 0;

	const ImageDescription m_Description;
	const TPixelFormat m_TexturePixelFormat;
	const long m_iUsageFlag;
};

#endif /* TEXTUREBASE_H_ */
