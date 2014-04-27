/**
 * This file describes Duke's IO API.
 *
 * First plugins register an IIODescriptor instance via the static
 * IODescriptors::registerDescriptor() function.
 *
 * Descriptors give informations about plugin capabilities. This allows Duke to
 * select the appropriate one depending on the context.
 * - READER_GENERAL_PURPOSE
 *     The plugin can read/write several formats. Those plugins will be tried
 *     after specialized ones.
 * - READER_READ_FROM_MEMORY
 *     File can be loaded by Duke and decoded by the plugin in-memory.
 * - READER_PERSISTENT
 *     The plugin has a state that is costly to create and we want to reuse it
 *     between calls. Typical case is a movie file where you want the same
 *     plugin instance to be used for all the images.
 *
 * Once created the plugin state can be checked by looking at the has/getError()
 * functions. If plugin creation succeeded Duke can fetch properties by reading
 * the plugin's attributes.
 *
 * Then the 'setup' plugin function will be called to initialize the metadata
 * - read : frame number to read, frame dimensions, data size, ...
 * - write: frame to write, open file on disk, ...
 *
 * If 'setup' call fails, user can check the getError() function for more
 * informations. If call succeeds, Duke is allowed to go on with read/write
 * functions.
 *
 * If plugin is persistent, pairs of setup/read or setup/write functions are
 * allowed. The plugin must configure it's state accordingly.
 */

#pragma once

#include <duke/imageio/RawPackedFrame.hpp>
#include <duke/base/Check.hpp>
#include <duke/base/NonCopyable.hpp>
#include <duke/base/StringUtils.hpp>

#include <cstddef>
#include <string>
#include <map>

namespace duke {

class IIODescriptor;

class IImageReader: public noncopyable {
protected:
    virtual bool doSetup(PackedFrameDescription& description, attribute::Attributes& attributes) = 0;
	const IIODescriptor * const m_pDescriptor;
	attribute::Attributes m_ReaderAttributes;
	std::string m_Error;

public:
    IImageReader(const attribute::Attributes& options, const IIODescriptor * pDescriptor) :
                    m_pDescriptor(pDescriptor), m_ReaderAttributes(std::move(options)) {
    }
	virtual ~IImageReader() {}
	inline bool hasError() const {
		return !m_Error.empty();
	}
	inline std::string getError() {
	    std::string copy(m_Error);
	    m_Error.clear();
		return copy;
	}
	inline const attribute::Attributes& getAttributes() {
		return m_ReaderAttributes;
	}
	inline const IIODescriptor * getDescriptor() const {
		return m_pDescriptor;
	}
    inline bool setup(RawPackedFrame& packedFrame) {
        return doSetup(packedFrame.description, packedFrame.attributes);
    }
	virtual const void* getMappedImageData() const {
		return nullptr;
	}
	virtual void readImageDataTo(void* pData) {
		m_Error = "Unsupported readImageDataTo";
	}
};

class IImageWriter: public noncopyable {
    attribute::Attributes m_WriterAttributes;
    std::string m_Error;
public:
	virtual ~IImageWriter() {}
    inline const std::string &getError() const {
        return m_Error;
    }
    inline attribute::Attributes& getAttributes() {
        return m_WriterAttributes;
    }
    virtual bool setup(const attribute::Attributes &input) {
        return false;
    }
};

}  // namespace duke

#include <vector>

namespace duke {

class IIODescriptor: public noncopyable {
public:
    enum class Capability {
        READER_READ_FROM_MEMORY, // Plugin can decode in-memory buffers
        READER_GENERAL_PURPOSE,  // Plugin can read several formats
        READER_FILE_SEQUENCE,    // Plugin will be instantiated for each frame
                                 // read will be parallel and out of order
    };
	virtual ~IIODescriptor() {}
	virtual const std::vector<std::string>& getSupportedExtensions() const = 0;
	virtual const char* getName() const = 0;
	virtual bool supports(Capability capability) const = 0;
	virtual IImageReader* getReaderFromFile(const attribute::Attributes& options, const char *filename) const {
		return nullptr;
	}
	virtual IImageReader* getReaderFromMemory(const attribute::Attributes& options, const void *pData, const size_t dataSize) const {
		return nullptr;
	}
	virtual IImageWriter* getWriterToFile(const attribute::Attributes& options, const char *filename) const {
		return nullptr;
	}
};

}  // namespace duke

#include <memory>
#include <map>
#include <deque>

namespace duke {

class IODescriptors: public noncopyable {
	std::vector<std::unique_ptr<IIODescriptor> > m_Descriptors;
	std::map<std::string, std::deque<IIODescriptor*>, ci_less > m_ExtensionToDescriptors;
public:
	bool registerDescriptor(IIODescriptor* pDescriptor);

	const std::deque<IIODescriptor*>& findDescriptor(const char* extension) const;

	bool isSupported(const char* extension) const;

	inline const std::vector<std::unique_ptr<IIODescriptor> >& getDescriptors() const {
		return m_Descriptors;
	}

	static IODescriptors& instance();
};

}  // namespace duke
