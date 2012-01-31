#ifndef IIMAGEBASE_H_
#define IIMAGEBASE_H_

#include "IResource.h"
#include "IImageDescriptor.h"
#include "Enums.h"
#include <dukeio/ImageDescription.h>
#include <vector>

class IImageBase : public IResource,
	public IImageDescriptor
{
friend class Image;
friend class DisplayableImage;

public: IImageBase( const ImageDescription& );
	virtual ~IImageBase();

	virtual const ImageDescription& getImageDescription() const;

	virtual const ::resource::Type getResourceType() const { return ::resource::IMAGE; }
	virtual std::size_t getSize() const { return sizeof(IImageBase) + m_Pixels.size(); }
private:
	const ImageDescription m_Description;
	typedef std::vector<unsigned char> VECTOR;
	VECTOR m_Pixels;
};

#endif /* IIMAGEBASE_H_ */
