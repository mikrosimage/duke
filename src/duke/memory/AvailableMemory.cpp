#include "AvailableMemory.hpp"

#ifdef _WIN32
#include <windows.h>
size_t getTotalSystemMemory() {
  MEMORYSTATUSEX status;
  status.dwLength = sizeof(status);
  GlobalMemoryStatusEx(&status);
  return status.ullTotalPhys;
}
#elif __APPLE__
#include <sys/sysctl.h>
size_t getTotalSystemMemory() {
  int mib[2];
  mib[0] = CTL_HW;
  mib[1] = HW_MEMSIZE;
  uint64_t memsize;
  size_t len = sizeof(memsize);
  sysctl(mib, 2, &memsize, &len, NULL, 0);
  return memsize;
}
#else
#include <unistd.h>
size_t getTotalSystemMemory() {
  long pages = sysconf(_SC_PHYS_PAGES);
  long page_size = sysconf(_SC_PAGE_SIZE);
  return pages * page_size;
}
#endif