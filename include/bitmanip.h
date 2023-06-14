// This header contains a few functions to be used if the intrinsic functions
// are unavailable. I doubt these are substantial or original enough to warrant
// copyright.
#ifndef BITMANIP_H_INCLUDED
#define BITMANIP_H_INCLUDED

#ifdef __GNUC__
#include <limits.h>
#define bit_length(x) (sizeof(int) * CHAR_BIT - __builtin_clz(x))
#else
unsigned int bit_length(unsigned int x) {
  unsigned int bits = 0;
  while (x) {
    bits++;
    x >>= 1;
  }
  return bits;
}
#endif  // __GNUC__

#ifdef __clang__
#define bit_reverse(x, n) (__builtin_bitreverse32(x) >> (32 - n))
#else
unsigned int bit_reverse(unsigned int x, unsigned int n) {
  unsigned int r = 0;
  for (unsigned int i = 0; i < n; ++i) {
    if (x & (1 << i)) r |= 1 << ((n - 1) - i);
  }
  return r;
}
#endif // __clang__

#endif  // BITMANIP_H_INCLUDED