#ifdef DUKE_OIIO

#include <duke/attributes/Attribute.hpp>
#include <duke/imageio/DukeIO.hpp>
#include <duke/gl/GL.hpp>

#include <OpenImageIO/imageio.h>

#include <algorithm>
#include <iterator>

using namespace std;
OIIO_NAMESPACE_USING;

struct OiioAttributeEntry {
    const char* name;
    TypeDesc type;
    int values;
    uint16_t datasize;
    char data;
};

template<typename T>
const char* appendAndAdvance(const char*& ptr, std::string& output) {
    output += std::to_string(*reinterpret_cast<const T*>(ptr));
    return ptr + sizeof(T);
}

const char* appendToString(const int type, const char* ptr, std::string& output) {
    switch (type) {
        case TypeDesc::UINT8:
            return appendAndAdvance<uint8_t>(ptr, output);
        case TypeDesc::INT8:
            return appendAndAdvance<int8_t>(ptr, output);
        case TypeDesc::UINT16:
            return appendAndAdvance<uint16_t>(ptr, output);
        case TypeDesc::INT16:
            return appendAndAdvance<int16_t>(ptr, output);
        case TypeDesc::UINT32:
            return appendAndAdvance<uint32_t>(ptr, output);
        case TypeDesc::INT32:
            return appendAndAdvance<int32_t>(ptr, output);
        case TypeDesc::UINT64:
            return appendAndAdvance<uint64_t>(ptr, output);
        case TypeDesc::INT64:
            return appendAndAdvance<int64_t>(ptr, output);
        case TypeDesc::FLOAT:
            return appendAndAdvance<float>(ptr, output);
        case TypeDesc::DOUBLE:
            return appendAndAdvance<double>(ptr, output);
        case TypeDesc::HALF:
        default:
            return nullptr;
    }
}

template<>
std::string TypedAttributeDescriptor<ParamValue>::dataToString(const AttributeEntry& entry) const {
    const OiioAttributeEntry& param = *reinterpret_cast<const OiioAttributeEntry*>(entry.data.data());
    const char* ptr = reinterpret_cast<const char*>(&param.data);
    string buffer;
    switch (param.type.basetype) {
        case TypeDesc::STRING: {
            buffer += '"';
            buffer += reinterpret_cast<const char*>(&param.data);
            buffer += '"';
            return buffer;
        }
        default: {
            if (param.values == 0) {
                return "N/A";
            }
            if (param.values == 1) {
                appendToString(param.type.basetype, ptr, buffer);
                return buffer;
            }
            buffer += '[';
            for (int i = 0; i < param.values; ++i) {
                if (i != 0) buffer += ',';
                ptr = appendToString(param.type.basetype, ptr, buffer);
            }
            buffer += ']';
            return buffer;
        }
    }
    return "N/A";
}

namespace attribute {

struct OiioAttribute : public AttributeKey {
    typedef ParamValue value_type;
    inline static ParamValue default_value() {
        return {};
    }
    virtual const char* name() const override {
        CHECK(!"");
        return "";
    }
    virtual const AttributeDescriptor* descriptor() const override {
        const static TypedAttributeDescriptor<ParamValue> descriptor(nullptr);
        return &descriptor;
    }
};

} // namespace attribute

namespace duke {

namespace {

const vector<string> A = { "A" };
const vector<string> RGB = { "R", "G", "B" };
const vector<string> RGBA = { "R", "G", "B", "A" };

GLuint getGLType(const TypeDesc &typedesc, const vector<string> &channels) {
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

size_t getTypeSize(const TypeDesc &typedesc) {
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

}  // namespace

class OpenImageIOReader : public IImageReader {
    unique_ptr<ImageInput> m_pImageInput;
    ImageSpec m_Spec;

public:
    OpenImageIOReader(const Attributes& options, const IIODescriptor *pDesc, const char *filename) :
                    IImageReader(options, pDesc), m_pImageInput(ImageInput::create(filename)) {
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
        if (m_Spec.channelnames != RGB && m_Spec.channelnames != RGBA && m_Spec.channelnames != A) {
            m_Error = "Can only handle RGB, RGBA and A images for now, was '";
            for (const auto& string : m_Spec.channelnames)
                m_Error += string;
            m_Error += "'";
            return;
        }
    }

    ~OpenImageIOReader() {
        if (m_pImageInput) m_pImageInput->close();
    }

    virtual bool doSetup(PackedFrameDescription& description, Attributes& attributes) override {
        description.width = m_Spec.width;
        description.height = m_Spec.height;
        description.glPackFormat = getGLType(m_Spec.format, m_Spec.channelnames);
        description.dataSize = m_Spec.width * m_Spec.height * m_Spec.nchannels * getTypeSize(m_Spec.format);

        for (const ParamValue& paramvalue : m_Spec.extra_attribs) {
            // For a string only the pointer is stored in the oiio attribute
            // we need to copy the whole data to prevent reading dangling pointers.
            const bool isString = paramvalue.type() == TypeDesc::STRING;
            const char* pData = isString ?
                            *reinterpret_cast<const char* const *>(paramvalue.data()) :
                            reinterpret_cast<const char*>(paramvalue.data());
            const size_t oiioDataSize = isString ? strlen(pData) + 1 : paramvalue.datasize();
            const size_t datasize = sizeof(OiioAttributeEntry) + oiioDataSize;
            AttributeData data(datasize);
            OiioAttributeEntry& asEntry = *reinterpret_cast<OiioAttributeEntry*>(data.data());
            asEntry.name = paramvalue.name().c_str();
            asEntry.type = paramvalue.type();
            asEntry.values = paramvalue.nvalues();
            asEntry.datasize = oiioDataSize;
            memcpy(&asEntry.data, pData, oiioDataSize);
            attributes.set(paramvalue.name().c_str(), attribute::OiioAttribute().descriptor(), std::move(data));
        }
        return true;
    }

    virtual void readImageDataTo(void* pData) {
        if (!m_pImageInput) return;
        if (!m_pImageInput->read_image(m_Spec.format, pData)) {
            m_Error = OpenImageIO::geterror();
            return;
        }
    }
};

class OpenImageIODescriptor : public IIODescriptor {
    vector<string> m_Extensions;
public:
    OpenImageIODescriptor() {
        string extensions;
        getattribute("extension_list", extensions);
        bool append = false;
        string current;
        for (const char c : extensions) {
            if (c == ';') append = false;
            if (c == ',' || c == ';') {
                m_Extensions.push_back(current);
                current.clear();
            } else if (append) current.push_back(c);
            if (c == ':') append = true;
        }
        m_Extensions.push_back(current);
    }
    virtual bool supports(Capability capability) const override {
        return capability == Capability::READER_GENERAL_PURPOSE || capability == Capability::READER_FILE_SEQUENCE;
    }
    virtual const vector<string>& getSupportedExtensions() const override {
        return m_Extensions;
    }
    virtual const char* getName() const override {
        return "OpenImageIO";
    }
    virtual IImageReader* getReaderFromFile(const Attributes& options, const char *filename) const override {
        return new OpenImageIOReader(options, this, filename);
    }
};

namespace {
bool registrar = IODescriptors::instance().registerDescriptor(new OpenImageIODescriptor());
}  // namespace

}  // namespace duke

#endif // DUKE_OIIO
