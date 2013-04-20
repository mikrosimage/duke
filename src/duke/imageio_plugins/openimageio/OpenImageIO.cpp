#ifdef DUKE_OIIO

#include <duke/imageio/DukeIO.hpp>
#include <duke/gl/GL.hpp>

#include <OpenImageIO/imageio.h>

#include <algorithm>
#include <iterator>

using namespace std;
OIIO_NAMESPACE_USING;

static const vector<string> A = { "A" };
static const vector<string> RGB = { "R", "G", "B" };
static const vector<string> RGBA = { "R", "G", "B", "A" };

static PrimitiveType getAttributeType(const TypeDesc &typedesc) {
	switch (typedesc.basetype) {
	case TypeDesc::UCHAR:
		return PrimitiveType::UCHAR;
	case TypeDesc::CHAR:
	case TypeDesc::STRING:
		return PrimitiveType::CHAR;
	case TypeDesc::USHORT:
		return PrimitiveType::USHORT;
	case TypeDesc::SHORT:
		return PrimitiveType::SHORT;
	case TypeDesc::UINT:
		return PrimitiveType::UINT;
	case TypeDesc::INT:
		return PrimitiveType::INT;
	case TypeDesc::FLOAT:
		return PrimitiveType::FLOAT;
	case TypeDesc::DOUBLE:
		return PrimitiveType::DOUBLE;
	default:
		return PrimitiveType::UNKNOWN;
	}
}
static GLuint getGLType(const TypeDesc &typedesc, const vector<string> &channels) {
	if (channels == A) {
		switch (typedesc.basetype) {
		case TypeDesc::UCHAR:
		case TypeDesc::CHAR:
			return GL_R8;
		case TypeDesc::USHORT:
		case TypeDesc::SHORT:
			return GL_R16;
		case TypeDesc::UINT:
			return GL_R32UI;
		case TypeDesc::INT:
			return GL_R32I;
		case TypeDesc::HALF:
			return GL_R16F;
		case TypeDesc::FLOAT:
			return GL_R32F;
		}
	} else if (channels == RGB) {
		switch (typedesc.basetype) {
		case TypeDesc::UCHAR:
		case TypeDesc::CHAR:
			return GL_RGB8;
		case TypeDesc::USHORT:
		case TypeDesc::SHORT:
			return GL_RGB16;
		case TypeDesc::UINT:
			return GL_RGB32UI;
		case TypeDesc::INT:
			return GL_RGB32I;
		case TypeDesc::HALF:
			return GL_RGB16F;
		case TypeDesc::FLOAT:
			return GL_RGB32F;
		}
	} else if (channels == RGBA) {
		switch (typedesc.basetype) {
		case TypeDesc::UCHAR:
		case TypeDesc::CHAR:
			return GL_RGBA8;
		case TypeDesc::USHORT:
		case TypeDesc::SHORT:
			return GL_RGBA16;
		case TypeDesc::UINT:
			return GL_RGBA32UI;
		case TypeDesc::INT:
			return GL_RGBA32I;
		case TypeDesc::HALF:
			return GL_RGBA16F;
		case TypeDesc::FLOAT:
			return GL_RGBA32F;
		}
	}
	return 0;
}

static size_t getTypeSize(const TypeDesc &typedesc) {
	switch (typedesc.basetype) {
	case TypeDesc::UCHAR:
	case TypeDesc::CHAR:
		return 1;
	case TypeDesc::USHORT:
	case TypeDesc::SHORT:
	case TypeDesc::HALF:
		return 2;
	case TypeDesc::UINT:
	case TypeDesc::INT:
	case TypeDesc::FLOAT:
		return 4;
	case TypeDesc::DOUBLE:
		return 8;
	default:
		return 0;
	}
}

namespace duke {

class OpenImageIOReader: public IImageReader {
	unique_ptr<ImageInput> m_pImageInput;
	ImageSpec m_Spec;

public:
	OpenImageIOReader(const IIODescriptor *pDesc, const char *filename) :
			IImageReader(pDesc), m_pImageInput(ImageInput::create(filename)) {
		if (!m_pImageInput) {
			m_Error = OpenImageIO::geterror();
			return;
		}
		if (!m_pImageInput->open(filename, m_Spec)) {
			m_Error = OpenImageIO::geterror();
			return;
		}
		if (m_Spec.depth > 1) {
			m_Error = "can't read volume images";
			return;
		}
		m_Description.width = m_Spec.width;
		m_Description.height = m_Spec.height;

		if (m_Spec.channelnames != RGB && m_Spec.channelnames != RGBA && m_Spec.channelnames != A) {
			m_Error = "Can only handle RGB, RGBA and A images for now, was '";
			for (const auto& string : m_Spec.channelnames)
				m_Error += string;
			m_Error += "'";
			return;
		}

		m_Description.glPackFormat = getGLType(m_Spec.format, m_Spec.channelnames);
		m_Description.dataSize = m_Spec.width * m_Spec.height * m_Spec.nchannels * getTypeSize(m_Spec.format);

//		m_Attributes.emplace_back("Orientation", 0);
		for (const ParamValue& paramvalue : m_Spec.extra_attribs) {
			const TypeDesc& oiio_type = paramvalue.type();
			const PrimitiveType type = getAttributeType(static_cast<TypeDesc::BASETYPE>(oiio_type.basetype));
			if (type == PrimitiveType::UNKNOWN)
				continue;
			const char* name = paramvalue.name().c_str();
			if (oiio_type.basetype == TypeDesc::STRING) {
				const char* pData = *reinterpret_cast<const char* const *>(paramvalue.data());
				m_Attributes.emplace_back(name, pData);
			} else {
				const size_t nvalues = paramvalue.nvalues() * oiio_type.aggregate;
				const char* pData = reinterpret_cast<const char*>(paramvalue.data());
				const size_t datasize = paramvalue.datasize();
				const size_t attr_nvalues = nvalues == 1 ? 0 : nvalues;
				m_Attributes.emplace_back(name, pData, datasize, type, attr_nvalues);
			}
		}
	}

	~OpenImageIOReader() {
		if (m_pImageInput)
			m_pImageInput->close();
	}

	virtual void readImageDataTo(void* pData) {
		if (!m_pImageInput)
			return;
		if (!m_pImageInput->read_image(m_Spec.format, pData)) {
			m_Error = OpenImageIO::geterror();
			return;
		}
	}
};

class OpenImageIODescriptor: public IIODescriptor {
	vector<string> m_Extensions;
public:
	OpenImageIODescriptor() {
		string extensions;
		getattribute("extension_list", extensions);
		bool append = false;
		string current;
		for (const char c : extensions) {
			if (c == ';')
				append = false;
			if (c == ',' || c == ';') {
				m_Extensions.push_back(current);
				current.clear();
			} else if (append)
				current.push_back(c);
			if (c == ':')
				append = true;
		}
		m_Extensions.push_back(current);
	}
	virtual bool supports(Capability capability) const {
		return capability == Capability::READER_GENERAL_PURPOSE;
	}
	virtual const vector<string>& getSupportedExtensions() const {
		return m_Extensions;
	}
	virtual const char* getName() const {
		return "OpenImageIO";
	}
	virtual IImageReader* getReaderFromFile(const char *filename) const {
		return new OpenImageIOReader(this, filename);
	}
};

namespace {
bool registrar = IODescriptors::instance().registerDescriptor(new OpenImageIODescriptor());
}  // namespace

}  // namespace duke

#endif // DUKE_OIIO
