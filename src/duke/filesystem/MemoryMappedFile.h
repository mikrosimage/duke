/*
 * MemoryMappedFile.h
 *
 *  Created on: Dec 16, 2012
 *      Author: Guillaume Chatelet
 */

#ifndef MEMORYMAPPEDFILE_H_
#define MEMORYMAPPEDFILE_H_

#include <duke/NonCopyable.h>
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

#endif /* MEMORYMAPPEDFILE_H_ */
