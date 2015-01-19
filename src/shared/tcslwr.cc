/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#include "wchar.h"

JARGON_NAMESPACE_BEGIN(shared)

tchar_t* _tcslwr(tchar_t* value) {
  tchar_t* result = value;
  for ( ; *value; value++) {
    *value = totlower(*value);
  }
  return result;
}

JARGON_NAMESPACE_END
