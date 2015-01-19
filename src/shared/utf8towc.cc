/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#include "wchar.h"

JARGON_NAMESPACE_BEGIN(shared)

size_t _utf8towc(wchar_t& target, const char *source) {
  int i; 
  int mask = 0;
  int result;
  unsigned char c = (unsigned char) *source;
  int length = 0;

  UTF8_PREPARE(c, mask, length);
  if (length == -1) {
    return 0;
  }
  TO_UTF8(result, source, i, mask, length);

  target = result;
  return length;
}

JARGON_NAMESPACE_END
