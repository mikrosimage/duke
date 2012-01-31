#ifndef ITEXTUREPROVIDER_H_
#define ITEXTUREPROVIDER_H_

#include "IImageDescriptor.h"
#include <dukeio/ImageDescription.h>

class ITextureBase;

class ITextureProvider : public IImageDescriptor
{
public:
	virtual ~ITextureProvider()              = 0;
	virtual ITextureBase* getTexture() const = 0;
};

#endif /* ITEXTUREPROVIDER_H_ */
