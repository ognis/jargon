/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#include "wchar.h"

JARGON_NAMESPACE_BEGIN(shared)

int64_t _tcstoi64(const tchar_t* value, tchar_t** end, int radix){
  if (radix < 2 || radix > 36) {
    return 0;
  }

  while (istspace (*value)) {
    ++value;
  }

  int sign = 1;
  if (value[0] == TLITERAL('+')) {
    value++;
  } else if (value[0] == TLITERAL('-')){
    sign = -1;
    value++;
  }
		
  *end = (tchar_t*)value;
  long r = -1;
  while ((r = TCSTOI64_RADIX(*end[0],radix)) >=0 && r < radix) {
    (*end)++;
  }

  tchar_t* p = (*end) - 1;
  int64_t result = 0;
  int position = 0;
  for ( ; p >= value; p-- ) {
    int i = TCSTOI64_RADIX(p[0], radix);
    if (position == 0) {
      result = i;
    }
    else
      result += (int64_t)pow((float_t)radix, (float_t)position) * i;
      position++;
  }
  return sign * result;
}

JARGON_NAMESPACE_END
