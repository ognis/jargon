/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#include "wchar.h"

JARGON_NAMESPACE_BEGIN(shared)

int _tcscasecmp(const tchar_t* target, const tchar_t* source){
  tchar_t c_target;
  tchar_t c_source;

  if (target == source) {
    return 0;
  }
    
  do {
    c_target = totlower((*(target++)));
    c_source = totlower((*(source++)));
  } while (c_target != L'\0' && (c_target == c_source));

  return (int)(c_target - c_source);
}

JARGON_NAMESPACE_END
