#pragma once

namespace  {

inline unsigned short bswap_16(unsigned short x) {
    return (x >> 8) | (x << 8);
}

inline unsigned int bswap_32(unsigned int x) {
    return (bswap_16(x & 0xffff) << 16) | (bswap_16(x >> 16));
}

}  // namespace
