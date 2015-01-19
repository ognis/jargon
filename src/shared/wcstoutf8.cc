/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#include "wchar.h"

JARGON_NAMESPACE_BEGIN(shared)

size_t _wcstoutf8(char* target, const wchar_t * source, size_t max){
  char* p= target;
  int i = 0;

  while (p < target + max - 1 && source[i] != 0) {
    p += _wctoutf8(p, source[i++]);
  }
  *p = '\0';

  return p - target;
}

JARGON_NAMESPACE_END

