/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#if defined (HAVE_WINDOWS_H)
#  include <windows.h>
#endif
#if defined (HAVE_TCHAR_H)
#  include <tchar.h>
#endif
#include <cstdint>
#include <cwchar>

/* Detects platform characteristics. */
int main(int argc, char *argv[]) {
#if defined (CHECK_IF_INT8_T_EXIST)
  int8_t x = 0.0;
#elif defined (CHECK_IF_INT16_T_EXIST)
  int16_t x = 0.0;
#elif defined (CHECK_IF_INT32_T_EXIST)
  int32_t x = 0.0;
#elif defined (CHECK_IF_INT64_T_EXIST)
  int64_t x = 0.0;
#elif defined (CHECK_IF_UINT8_T_EXIST)
  uint8_t x = 0.0;
#elif defined (CHECK_IF_UINT16_T_EXIST)
  uint16_t x = 0.0;
#elif defined (CHECK_IF_UINT32_T_EXIST)
  uint32_t x = 0.0;
#elif defined (CHECK_IF_UINT64_T_EXIST)
  uint64_t x = 0.0;
#elif defined (CHECK_IF_WORD_EXIST)
  WORD x = 0.0;
#elif defined (CHECK_IF_DWORD_EXIST)
  DWORD x = 0.0;
#elif defined (CHECK_IF_TCHAR_EXIST)
  TCHAR x = 0.0;
#endif
  return 0;
}

