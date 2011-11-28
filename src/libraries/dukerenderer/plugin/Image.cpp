#include "Image.h"
#include "IFactory.h"
#include "Enums.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
using namespace ::duke::protocol;

Image::Image(IFactory& factory, const string& name, const ImageDescription& imageDescription) {
    m_pImage = factory.getResourceManager().get<IImageBase> (::resource::IMAGE, name);
    if (!m_pImage) {
        m_pImage.reset(new IImageBase(imageDescription));
        if (!name.empty())
            factory.getResourceManager().add(name, m_pImage);

    }
    // now updating image data
    if (imageDescription.imageDataSize != 0) {
        assert( m_pImage->m_Pixels.size() >= imageDescription.imageDataSize );
        memcpy(m_pImage->m_Pixels.data(), imageDescription.pImageData, imageDescription.imageDataSize);
    }
}

Image::Image(IFactory& factory, const string& name) {
    m_pImage = factory.getResourceManager().safeGet<IImageBase> (::resource::IMAGE, name);
}

Image::~Image() {
}

const ImageDescription& Image::getImageDescription() const {
    assert( m_pImage );
    return m_pImage->getImageDescription();
}

void Image::dump(const string& filename) const {
    const ImageDescription &description(getImageDescription());

    const size_t elements = description.width * description.height;
    std::vector<unsigned char> data;
    data.reserve(elements * 3 * sizeof(char));
    const unsigned char* pSrcData = m_pImage->m_Pixels.data();
    for (size_t i = 0; i < elements; ++i) {
        data.push_back(pSrcData[2]);
        data.push_back(pSrcData[1]);
        data.push_back(pSrcData[0]);
        pSrcData += 4;
    }

    ostringstream headerStream;
    headerStream << "P6" << endl;
    headerStream << description.width << " " << description.height << endl;
    headerStream << "255" << endl;
    const string header = headerStream.str();

    //    const HANDLE file = CreateFile(filename.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING, NULL);
    //    DWORD read;
    //    WriteFile(file, header.data(), header.size(), &read, NULL);
    //    WriteFile(file, data.data(), data.size(), &read, NULL);
    //    CloseHandle(file);
    ofstream file(filename.c_str(), ios_base::binary | ios_base::out | ios_base::trunc);
    file.write(header.data(), header.size());
    file.write(reinterpret_cast<char*> (data.data()), data.size());
    file.close();
}

void Image::dump(Texture &texture) const {
    texture.Clear();

    const ImageDescription &description(getImageDescription());
    texture.set_width(description.width);
    texture.set_height(description.height);
    texture.set_depth(description.depth);
    texture.set_format(Enums::Get(description.format));
//    texture.set_format(Texture_TextureFormat_R8G8B8A8);//Enums::Get(description.format));
    texture.set_data(m_pImage->m_Pixels.data(), m_pImage->m_Pixels.size());
}

//TPixelFormat Image::getFormat() const {
//	return m_Format;
//}
//
//const unsigned char* Image::getData() const {
//	return &m_Pixels[0];
//}

//void Image::loadFromFile(const std::string& filename) {
//	//	std::auto_ptr<Image> image = CMediaManager::Instance().LoadMediaFromFile<image> (Filename);
//	//
//	//	copyImage(*image);
//	throw std::runtime_error("unsupported operation exception");
//}
//
//void Image::fill(const Color& Color) {
//	setPixel(0, 0, Color);
//
//	const unsigned int bpp = GetBytesPerPixel(m_Format);
//	std::vector<unsigned char>::iterator begin = m_Pixels.begin();
//	for (std::vector<unsigned char>::iterator i = begin + bpp; i != m_Pixels.end(); i += bpp)
//		std::copy(begin, begin + bpp, i);
//}

//void Image::setPixel(int x, int y, const unsigned char* pixel) {
//	assert(m_pImage);
//	m_pImage->setPixel(x, y, pixel);
//}
//
//void Image::setPixel(int x, int y, const Color& color) {
//	const unsigned char components[4] = { color.getBlue(), color.getGreen(), color.getRed(), color.getAlpha() };
//	setPixel(x, y, components);
//	//ConvertPixel(PXF_A8R8G8B8, components, m_Format, pix);
//}
//
//void Image::getPixel(int x, int y, unsigned char* Pix) const {
//	const unsigned char* begin = &m_Pixels[(x + y * m_Size.x) * GetBytesPerPixel(m_Format)];
//	std::copy(begin, begin + GetBytesPerPixel(m_Format), Pix);
//}
//
//Color Image::getPixel(int x, int y) const {
//	unsigned char components[4];
//
//	const unsigned char* Pix = &m_Pixels[(x + y * m_Size.x) * GetBytesPerPixel(m_Format)];
//
//	ConvertPixel(m_Format, Pix, PXF_A8R8G8B8, components);
//
//	return Color(components[2], components[1], components[0], components[3]);
//}

//void Image::copyImage(const Image& src) {
//	if (m_Size == src.m_Size) {
//		if (m_Format == src.m_Format) {
//			m_Pixels = src.m_Pixels;
//		} else {
//			const unsigned char* pSrcPix = &src.m_Pixels[0];
//			unsigned char* pDestPix = &m_Pixels[0];
//			unsigned int srcBpp = GetBytesPerPixel(src.m_Format);
//			unsigned int sestBpp = GetBytesPerPixel(m_Format);
//
//			for (int i = 0; i < m_Size.x; ++i)
//				for (int j = 0; j < m_Size.y; ++j) {
//					ConvertPixel(src.m_Format, pSrcPix, m_Format, pDestPix);
//					pSrcPix += srcBpp;
//					pDestPix += sestBpp;
//				}
//		}
//	} else {
//		TVector2F Step(static_cast<float> (src.m_Size.x) / m_Size.x, static_cast<float> (src.m_Size.y) / m_Size.y);
//
//		if (m_Format == src.m_Format) {
//			unsigned char pixel[16];
//			for (int i = 0; i < m_Size.x; ++i)
//				for (int j = 0; j < m_Size.y; ++j) {
//					src.getPixel(static_cast<int> (i * Step.x), static_cast<int> (j * Step.y), pixel);
//					setPixel(i, j, pixel);
//				}
//		} else {
//			unsigned char srcPix[16], destPix[16];
//			for (int i = 0; i < m_Size.x; ++i)
//				for (int j = 0; j < m_Size.y; ++j) {
//					src.getPixel(static_cast<int> (i * Step.x), static_cast<int> (j * Step.y), srcPix);
//					ConvertPixel(src.m_Format, srcPix, m_Format, destPix);
//					setPixel(i, j, destPix);
//				}
//		}
//	}
//}
//
//Image Image::subImage(const Rect& rect) const {
//	assert(Rect(0, 0, m_Size.x, m_Size.y).intersects(rect) == INT_IN);
//
//	Image image(TVector2I(rect.width(), rect.height()), m_Format);
//
//	const unsigned char* pSrc = &m_Pixels[(rect.left() + rect.top() * m_Size.x) * GetBytesPerPixel(m_Format)];
//	unsigned char* pDest = &image.m_Pixels[0];
//	const unsigned int srcPitch = m_Size.x * GetBytesPerPixel(m_Format);
//	const unsigned int destPitch = image.m_Size.x * GetBytesPerPixel(image.m_Format);
//
//	for (int i = rect.left(); i < rect.right(); ++i) {
//		std::copy(pSrc, pSrc + destPitch, pDest);
//		pSrc += srcPitch;
//		pDest += destPitch;
//	}
//
//	return image;
//}
//
//void Image::flip() {
//	const int bpp = GetBytesPerPixel(m_Format);
//	for (int j = 0; j < m_Size.y / 2; ++j)
//		std::swap_ranges(&m_Pixels[j * m_Size.x * bpp], &m_Pixels[(j + 1) * m_Size.x * bpp - 1], &m_Pixels[(m_Size.y - j - 1) * m_Size.x * bpp]);
//
//}
//
//void Image::mirror() {
//	const int bpp = GetBytesPerPixel(m_Format);
//	for (int i = 0; i < m_Size.x / 2; ++i)
//		for (int j = 0; j < m_Size.y; ++j)
//			std::swap_ranges(&m_Pixels[(i + j * m_Size.x) * bpp], &m_Pixels[(i + j * m_Size.x + 1) * bpp], &m_Pixels[(m_Size.x - i - 1 + j * m_Size.x) * bpp]);
//}
