#pragma once

namespace  {

inline unsigned short bswap_16(unsigned short x) {
    return (x >> 8) | (x << 8);
}

inline unsigned int bswap_32(unsigned int x) {
    return (bswap_16(x & 0xffff) << 16) | (bswap_16(x >> 16));
}

inline unsigned long long bswap_64(unsigned long long x) {
    return (((unsigned long long) bswap_32(x & 0xffffffffull)) << 32) | (bswap_32(x >> 32));
}

}  // namespace
