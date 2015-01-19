/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#include "wchar.h"

JARGON_NAMESPACE_BEGIN(shared)

size_t _utf8towcs(wchar_t * target, const char * source, size_t max) {
  char *sp = const_cast<char*>(source);
  wchar_t *rp = target;

  while (rp < target + max && *sp != 0) {
    size_t r = _utf8towc(*rp, sp);
    if (r == 0) {
      return 0;
    }
    sp += r;
    rp++;
  }

  size_t result = sp - source;
  if (result < max) {
    *rp = '\0';
  }

  return result;
}

JARGON_NAMESPACE_END

