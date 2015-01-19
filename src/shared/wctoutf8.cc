/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#include "wchar.h"

JARGON_NAMESPACE_BEGIN(shared)

size_t _wctoutf8(char* target, const wchar_t source) {
  unsigned long c = source;
  unsigned char length = 0;
  int first;
  int i;

  if (c < 0x80) {
    first = 0;
    length = 1;
  } else if (c < 0x800) {
    first = 0xc0;
    length = 2;
  } else if (c < 0x10000) {
    first = 0xe0;
    length = 3;
  } else if (c < 0x200000) {
    first = 0xf0;
    length = 4;
  } else if (c < 0x4000000) {
    first = 0xf8;
    length = 5;
  } else {
    first = 0xfc;
    length = 6;
  }

  if (target) {
    for (i = length - 1; i > 0; --i) {
      target[i] = (char)((c & 0x3f) | 0x80);
      c >>= 6;
    }
    target[0] = (char)(c | first);
  }
  return length;
}

JARGON_NAMESPACE_END
