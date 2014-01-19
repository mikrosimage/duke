#pragma once

namespace  {

template<typename T>
inline T swap(const T& arg, bool wantBig) {
#if (BYTE_ORDER == BIG_ENDIAN) == wantBig
    return arg;           // no byte-swapping needed
#else                     // swap bytes
    T ret;

    char* dst = reinterpret_cast<char*>(&ret);
    const char* src = reinterpret_cast<const char*>(&arg + 1);

    for (size_t i = 0; i < sizeof(T); ++i)
        *dst++ = *--src;

    return ret;
#endif
}

}  // namespace
