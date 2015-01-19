/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#include "wchar.h"

JARGON_NAMESPACE_BEGIN(shared)

tchar_t* _i64tot(int64_t value, tchar_t* target, int radix) {
  uint64_t residual;
  int digit;
  tchar_t buffer[65];
  tchar_t* position;
  bool negate = false;

  if (value < 0 && radix == 10) {
    negate = true;
    residual = - value;
  } else {
    negate = false;
    residual = value;
  }

  position = &buffer[64];
  *position = '\0';

  do {
    digit = (int)(residual % radix);
    residual = residual / radix;
    if (digit < 10) {
      *--position = '0' + digit;
    } else {
      *--position = 'a' + digit - 10;
    }
  } while (residual != 0L);

  if (negate) {
    *--position = '-';
  }

  tcsncpy(target, position, &buffer[64] - position + 1);
  return target;
}

JARGON_NAMESPACE_END
