/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#if defined (HAVE_WINDOWS_H)
#  include <windows.h>
#endif
#include <cstdlib>
#include <string>

/* Detects platform characteristics. */
int main(int argc, char *argv[]) {
#if defined (CHECK_IF_STRCASECMP_F_EXIST)
  char* foo = "FOO";
  char* bar = "BAR";
  strcasecmp(foo, bar);
#elif defined (CHECK_IF_STRLWR_F_EXIST)
  char* foo = "FOO";
  char* bar = "BAR";
  strlwr(foo);
  strlwr(bar);
#elif defined (CHECK_IF_LLTOA_F_EXIST)
  long long foo = 0x12345678;
  char* bar;
  lltoa(foo, bar, 10);
#elif defined (CHECK_IF_STRTOLL_F_EXIST)
  long long foo;
  char* bar = "0x12345678";
  char* ptr;
  foo = strtoll(bar, ptr, 16);
#endif
  return 0;
}

