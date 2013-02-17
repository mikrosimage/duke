/*
 * DukeIO.h
 *
 *  Created on: Dec 15, 2012
 *      Author: Guillaume Chatelet
 */

#ifndef DUKEIO_H_
#define DUKEIO_H_

#include <duke/imageio/RawPackedFrame.h>
#include <duke/NonCopyable.h>

#include <cstddef>
#include <string>
#include <map>

namespace duke {

class IIODescriptor;

class IImageReader: public noncopyable {
private:
	RawPackedFrame m_PackedFrame;
protected:
	const IIODescriptor * const m_pDescriptor;
	PackedFrameDescription &m_Description;
	Attributes &m_Attributes;
	Attributes m_ReaderAttributes;
	std::string m_Error;
public:
	IImageReader(const IIODescriptor * pDescriptor) :
			m_pDescriptor(pDescriptor), m_Description(m_PackedFrame.description), m_Attributes(m_PackedFrame.attributes) {
	}
	virtual ~IImageReader() {
	}
	inline bool hasError() const {
		return !m_Error.empty();
	}
	inline const std::string &getError() const {
		return m_Error;
	}
	inline Attributes& getReaderAttributes() {
		return m_ReaderAttributes;
	}
	inline const RawPackedFrame& getRawPackedFrame() const {
		return m_PackedFrame;
	}
	inline const IIODescriptor * getDescriptor() const {
		return m_pDescriptor;
	}
	virtual const void* getMappedImageData() const {
		return nullptr;
	}
	virtual void readImageDataTo(void* pData) {
		m_Error = "Unsupported readImageDataTo";
	}
};

class IImageWriter: public noncopyable {
public:
	virtual ~IImageWriter() {
	}
};

}  // namespace duke

#include <vector>

namespace duke {

class IIODescriptor: public noncopyable {
public:
	enum class Capability {
		READER_READ_FROM_MEMORY, READER_ALLOCATES_MEMORY, READER_GENERAL_PURPOSE
	};
	virtual ~IIODescriptor() {
	}
	virtual const std::vector<std::string>& getSupportedExtensions() const = 0;
	virtual const char* getName() const = 0;
	virtual bool supports(Capability capability) const = 0;
	virtual IImageReader* getReaderFromFile(const char *filename) const {
		return nullptr;
	}
	virtual IImageReader* getReaderFromMemory(const void *pData, const size_t dataSize) const {
		return nullptr;
	}
	virtual IImageWriter* getWriterToFile(const char *filename) const {
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
	std::map<std::string, std::deque<IIODescriptor*> > m_ExtensionToDescriptors;
public:
	bool registerDescriptor(IIODescriptor* pDescriptor);
	const std::deque<IIODescriptor*>& findDescriptor(const char* extension) const;

	static IODescriptors& instance();
};

}  // namespace duke

#endif /* DUKEIO_H_ */
