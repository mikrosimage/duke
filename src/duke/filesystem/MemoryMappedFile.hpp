#pragma once

#include <duke/NonCopyable.hpp>
#include <cstddef>

struct MemoryMappedFile: public noncopyable {
	MemoryMappedFile(const char* filename);
	~MemoryMappedFile();
	operator bool() const {
		return !m_Error;
	}
	void *pFileData;
	size_t fileSize;
private:
	bool m_Error;
};
