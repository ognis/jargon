/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#if defined (HAVE_WINDOWS_H)
#  include <windows.h>
#endif
#include <cstdarg>
#include <cstdio>
#include <cwchar>

#if defined (CHECK_IF_VSNWPRINTF_F_EXIST)
void print_wrapper(const wchar_t * format, ...) {
  wchar_t buffer[256];
  va_list args;
  va_start (args, format);
  vsnwprintf(buffer, 256, format, args);
  va_end ( args );
}
#endif

/* Detects platform characteristics. */
int main(int argc, char *argv[]) {
#if defined (CHECK_IF_SNWPRINTF_F_EXIST)
  wchar_t foo[128] = {'\0'};
  wchar_t* bar = L"BAR";
  wchar_t* buzz = L"BUZZ";
  snwprintf(foo, 128, L"%s %s", bar, buzz); 
#elif defined (CHECK_IF_WPRINTF_F_EXIST)
  wchar_t* foo = L"FOO";
  wprintf(L"%s", foo);
#elif defined (CHECK_IF_VSNWPRINTF_F_EXIST)
  wchar_t* foo = L"FOO %s";
  wchar_t* bar = L"BAR";
  print_wrapper(foo, bar);
#endif
  return 0;
}

