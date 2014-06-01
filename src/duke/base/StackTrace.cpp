#include "duke/base/StackTrace.hpp"

#ifdef __GNUC__

#include <cstdio>
#include <execinfo.h>
#include <unistd.h>

void printStackTrace() {
  void *array[10];
  const size_t size = backtrace(array, 10);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
}

#else

// TODO(implement on other platforms)
void printStackTrace() {}

#endif
