/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#include "wchar.h"
#include "../util/misc.h"

#ifndef JARGON_ASCII_MODE

JARGON_NAMESPACE_BEGIN(shared)

double _tcstod(const tchar_t *value, tchar_t **end) {
  int32_t length = tcslen(value) + 1;
  char* char_value = JARGON_NEW_ARRAY(char, length);
  char* char_end = NULL;
  STRCPY_TtoA(char_value, value, length);
  double result = strtod(char_value, &char_end);
  *end = (tchar_t*)value + (char_end - char_value);
  JARGON_DEL_ARRAY(char_value);
  return result;
}

JARGON_NAMESPACE_END

#endif
