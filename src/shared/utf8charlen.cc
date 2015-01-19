/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#include "wchar.h"

JARGON_NAMESPACE_BEGIN(shared)

size_t _utf8charlen(const unsigned char p) {
  int mask = 0;
  int length = 0;
  UTF8_PREPARE(p, mask, length);
  return length;
}

JARGON_NAMESPACE_END

