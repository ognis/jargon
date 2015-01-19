/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#if defined (HAVE_WINDOWS_H)
#  include <windows.h>
#endif
#include <cstdlib>
#include <cwchar>

/* Detects platform characteristics. */
int main(int argc, char *argv[]) {
#if defined (CHECK_IF_WCSCASECMP_F_EXIST)
  wchar_t* foo = L"FOO";
  wchar_t* bar = L"BAR";
  wcscasecmp(foo, bar);
#elif defined (CHECK_IF_WCSLWR_F_EXIST)
  wchar_t* foo = L"FOO";
  wchar_t* bar;
  bar = wcslwr(foo);
#elif defined (CHECK_IF_LLTOW_F_EXIST)
  long long foo = 0x12345678;
  wchar_t* bar;
  lltow(foo, bar, 10);
#elif defined (CHECK_IF_WCSTOLL_F_EXIST)
  long long foo;
  wchar_t* bar = L"0x12345678";
  wchar_t* ptr;
  foo = wcstoll(bar, ptr, 16);
#elif defined (CHECK_IF_WCSTOD_F_EXIST)
  double foo;
  wchar_t* bar = L"1234567890";
  wchar_t* ptr;
  foo = wcstoll(bar, ptr);
#elif defined (CHECK_IF_WCSDUP_F_EXIST)
  wchar_t* foo = L"FOO";
  wchar_t* bar = L"BAR";
  bar = wcsdup(foo);
#endif
  return 0;
}

