/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#if defined (HAVE_WINDOWS_H)
#  include <windows.h>
#endif
#include <unistd.h>

/* Detects platform characteristics. */
int main(int argc, char *argv[]) {
  usleep(1);
  return 0;
}
