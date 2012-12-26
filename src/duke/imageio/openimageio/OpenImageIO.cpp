#ifdef DUKE_OIIO

#include <duke/imageio/DukeIO.h>

#include <OpenImageIO/imageio.h>

#include <GL/gl.h>

#include <algorithm>
#include <iterator>

using namespace std;
OIIO_NAMESPACE_USING;

static GLuint getGLType(const TypeDesc &typedesc) {
	switch (typedesc.basetype) {
	case TypeDesc::UCHAR:
		return GL_UNSIGNED_BYTE;
	case TypeDesc::CHAR:
		return GL_BYTE;
	case TypeDesc::USHORT:
		return GL_UNSIGNED_SHORT;
	case TypeDesc::SHORT:
		return GL_SHORT;
	case TypeDesc::UINT:
		return GL_UNSIGNED_INT;
	case TypeDesc::INT:
		return GL_INT;
	case TypeDesc::HALF:
		return GL_HALF_FLOAT;
	case TypeDesc::FLOAT:
		return GL_FLOAT;
	case TypeDesc::DOUBLE:
		return GL_DOUBLE;
	default:
		return 0;
	}
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
		static const vector<string> RGB = { "R", "G", "B" };
		static const vector<string> RGBA = { "R", "G", "B", "A" };
		if (m_Spec.channelnames != RGB && m_Spec.channelnames != RGBA) {
			m_Error = "Can only handle RGB and RGBA images for now";
			return;
		}

		m_Description.glType = getGLType(m_Spec.format);
		m_Description.glFormat = m_Spec.nchannels == 4 ? GL_RGBA : GL_RGB;
		m_Description.dataSize = m_Spec.width * m_Spec.height * m_Spec.nchannels * getTypeSize(m_Spec.format);

		m_Attributes.emplace_back("Orientation", 0);
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

#endif // DUKE_OIIO
