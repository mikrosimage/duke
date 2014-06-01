#pragma once

template <typename T>
inline T swap(const T& arg, bool wantBigEndian) {
  if (sizeof(T) == 1) return arg;

  constexpr bool isBigEndian = (BYTE_ORDER == BIG_ENDIAN);
  if (isBigEndian == wantBigEndian) return arg;  // no byte-swapping needed

  T ret;
  char* dst = reinterpret_cast<char*>(&ret);
  const char* src = reinterpret_cast<const char*>(&arg + 1);
  for (size_t i = 0; i < sizeof(T); ++i) *dst++ = *--src;
  return ret;
}
