#ifndef IMAGE_H_
#define IMAGE_H_

#include "Enums.h"
#include "utils/Color.h"
#include "math/Vector2.h"
#include "IImageDescriptor.h"
#include "IImageBase.h"
#include <boost/shared_ptr.hpp>
#include <string>

class IFactory;

class Image : public IImageDescriptor {
private:
    friend class DisplayableImage;

public:
    // create or update an image from ImageDescription
    Image(IFactory& factory, const std::string& name, const ImageDescription& imageDescription);

    // fetch registered image with name
    Image(IFactory& factory, const std::string& name);

    void dump(const std::string& filename) const;
    void dump(::protocol::duke::Texture &texture) const;

    virtual ~Image();

    virtual const ImageDescription& getImageDescription() const;

protected:
    ::boost::shared_ptr<IImageBase> m_pImage;
};

#endif // IMAGE_H_
