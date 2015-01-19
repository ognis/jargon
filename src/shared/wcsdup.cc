/*
 * Copyright (C) 2014- Shingo OKAWA
 */
#include "wchar.h"

const tchar_t* JARGON_BLANK_TSTRING = TLITERAL("");
const char* JARGOON_BLANK_STRING = "";

JARGON_NAMESPACE_BEGIN(shared)

wchar_t* _wcsdup(const wchar_t* value) {
  size_t length = wcslen(value);
  wchar_t* result = (wchar_t*)malloc( (length + 1) * sizeof(wchar_t) );
  wcscpy(result, value);
  return result;
}

JARGON_NAMESPACE_END
