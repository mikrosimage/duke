#include "Enums.h"
#include "DisplayableImage.h"
#include "IFactory.h"
#include "ITextureBase.h"

#include <iostream>
using namespace std;

ImageDescription getImageDescriptionFrom(const ::duke::protocol::Texture& texture) {
    ImageDescription description;

    description.width = texture.width();
    description.height = texture.height();
    description.depth = texture.depth();
    if (texture.has_data()) {
        description.pImageData = texture.data().c_str();
        description.imageDataSize = texture.data().size();
    }
    description.format = Enums::Get(texture.format());
    return description;
}

DisplayableImage::DisplayableImage(IFactory& factory, const ::duke::protocol::Texture& texture) :
    m_Image(factory, texture.name(), getImageDescriptionFrom(texture)) {
    m_pTexture = factory.getResourceManager().get<ITextureBase> (::resource::TEXTURE, texture.name());
    if (!m_pTexture) {
        m_pTexture.reset(factory.createTexture(getImageDescription()));
        if (!texture.name().empty())
            factory.getResourceManager().add(texture.name(), m_pTexture);
    }
    updateTexture();
}

DisplayableImage::DisplayableImage(IFactory& factory, const std::string& name) :
    m_Image(factory, name) {
    m_pTexture = factory.getResourceManager().get<ITextureBase> (::resource::TEXTURE, name);
}

const ImageDescription& DisplayableImage::getImageDescription() const {
    return m_Image.getImageDescription();
}

DisplayableImage::~DisplayableImage() {
}

void DisplayableImage::updateTexture() {
    IImageBase* pImage = m_Image.m_pImage.get();

    assert( pImage );
    assert( m_pTexture );
    m_pTexture->update(pImage->m_Description, &pImage->m_Pixels[0]);
}

ITextureBase* DisplayableImage::getTexture() const {
    return m_pTexture.get();
}

