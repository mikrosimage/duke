#include "MemoryMappedFile.hpp"

#include "duke/base/Check.hpp"

#include <cstdio>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

struct RaiiFile : public noncopyable {
  RaiiFile(const char* filename) : fd(open(filename, O_RDONLY)) {}
  ~RaiiFile() {
    if (fd != -1) close(fd);
  }
  operator bool() const { return fd != -1; }
  const int fd;
};

MemoryMappedFile::MemoryMappedFile(const char* filename) : pFileData(nullptr), fileSize(0), m_Error(true) {
  CHECK(filename);

  RaiiFile file(filename);
  if (!file) return;

  struct stat sb;
  if (fstat(file.fd, &sb) == -1) return;

  if (!S_ISREG(sb.st_mode)) return;

  fileSize = sb.st_size;
  pFileData = mmap(0, fileSize, PROT_READ, MAP_SHARED, file.fd, 0);
  if (pFileData == MAP_FAILED) return;

  m_Error = false;
}

MemoryMappedFile::~MemoryMappedFile() {
  if (pFileData != MAP_FAILED) munmap(pFileData, fileSize);
}
