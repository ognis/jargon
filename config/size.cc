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
#include <cstdio>
#include <cwchar>

/* Detects platform characteristics. */
int main(int argc, char *argv[]) {
  printf("%d", sizeof(CHECKING_TYPE));
  return 0;
}
