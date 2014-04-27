#pragma once

#include <cstdlib>
#include <cstdio>
#include <sstream>

#ifdef __GNUC__
#define DUKE_LIKELY(x) (__builtin_expect((x), 1))
#define DUKE_UNLIKELY(x) (__builtin_expect((x), 0))
#else
#define DUKE_LIKELY(x) (x)
#define DUKE_UNLIKELY(x) (x)
#endif

#define TRACE_MSG(X) fprintf(stderr, "CHECK FAILED '" #X "' in %s() [%s:%d]\n", __func__, __FILE__, __LINE__)

struct Tracer {
  Tracer(const char* cond, const char* func, const char* file, const int line) {
    msg << "CHECK FAILED '" << cond << "' in " << func << "() [" << file << ":" << line << "] ";
  }
  std::ostream& getStream() { return msg; }
  ~Tracer() {
    fprintf(stderr, "%s\n", msg.str().c_str());
    abort();
  }

 private:
  std::ostringstream msg;
};

#define CHECK(X) \
  if (DUKE_UNLIKELY(!(X))) Tracer(#X, __func__, __FILE__, __LINE__).getStream()
