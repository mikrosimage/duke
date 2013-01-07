/*
 * DukeIO.h
 *
 *  Created on: Dec 15, 2012
 *      Author: Guillaume Chatelet
 */

#ifndef DUKEIO_H_
#define DUKEIO_H_

#include "Attributes.h"
#include <duke/NonCopyable.h>

#include <cstddef>
#include <string>
#include <map>

struct ImageDescription {
	size_t width, height, depth;
	size_t glFormat; // GL_RED, GL_RG, GL_RGB, GL_BGR, GL_RGBA, and GL_BGRA.
	size_t glType; // GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, GL_INT, GL_FLOAT, GL_UNSIGNED_INT_10_10_10_2, and GL_UNSIGNED_INT_2_10_10_10_REV
	size_t dataSize;
	ImageDescription() :
			width(0), height(0), depth(0), glFormat(0), glType(0), dataSize(0) {
	}
};

class IIODescriptor;

class IImageReader: public noncopyable {
protected:
	const IIODescriptor * const m_pDescriptor;
	ImageDescription m_Description;
	std::string m_Error;
	Attributes m_Attributes;
public:
	IImageReader(const IIODescriptor * pDescriptor) :
			m_pDescriptor(pDescriptor) {
	}
	virtual ~IImageReader() {
	}
	inline bool hasError() const {
		return !m_Error.empty();
	}
	inline const std::string &getError() const {
		return m_Error;
	}
	inline ImageDescription getDescription() const {
		return m_Description;
	}
	inline const IIODescriptor * getDescriptor() const {
		return m_pDescriptor;
	}
	inline const Attributes& getAttributes() const {
		return m_Attributes;
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

#include <vector>
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

#include <memory>
#include <map>
#include <deque>
class IODescriptors: public noncopyable {
	std::vector<std::unique_ptr<IIODescriptor> > m_Descriptors;
	std::map<std::string, std::deque<IIODescriptor*> > m_ExtensionToDescriptors;
public:
	bool registerDescriptor(IIODescriptor* pDescriptor);
	const std::deque<IIODescriptor*>& findDescriptor(const char* extension) const;

	static IODescriptors& instance();
};

#endif /* DUKEIO_H_ */
