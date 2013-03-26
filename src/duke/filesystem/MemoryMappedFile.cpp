#include "MemoryMappedFile.hpp"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

struct RaiiFile: public noncopyable {
	RaiiFile(const char* filename) :
			fd(open(filename, O_RDONLY)) {
	}
	~RaiiFile() {
		if (fd != -1)
			close(fd);
	}
	operator bool() const {
		return fd != -1;
	}
	const int fd;
};

MemoryMappedFile::MemoryMappedFile(const char* filename) :
		pFileData(nullptr), fileSize(0), m_Error(false) {
	RaiiFile file(filename);
	if (!file) {
		m_Error = true;
		return;
	}
	struct stat sb;
	if (fstat(file.fd, &sb) == -1) {
		m_Error = true;
		return;
	}
	if (!S_ISREG (sb.st_mode)) {
		m_Error = true;
		return;
	}
	fileSize = sb.st_size;
	pFileData = mmap(0, fileSize, PROT_READ, MAP_SHARED, file.fd, 0);
	if (pFileData == MAP_FAILED ) {
		m_Error = true;
		return;
	}
}

MemoryMappedFile::~MemoryMappedFile() {
	if (pFileData != MAP_FAILED )
		munmap(pFileData, fileSize);
}

