#pragma once

#include <cstdlib>
#include <cstdio>

#ifdef __GNUC__
#define DUKE_LIKELY(x)   (__builtin_expect((x), 1))
#define DUKE_UNLIKELY(x) (__builtin_expect((x), 0))
#else
#define DUKE_LIKELY(x)   (x)
#define DUKE_UNLIKELY(x) (x)
#endif

#define TRACE_MSG(X) fprintf(stderr, \
                             "CHECK FAILED '"#X"' in %s() [%s:%d]\n", \
                             __func__, \
                             __FILE__, \
                             __LINE__)
#define CHECK(X) if(DUKE_UNLIKELY(!(X))) { TRACE_MSG(X); abort(); }
