/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#if defined (HAVE_WINDOWS_H)
#  include <windows.h>
#endif
#include <fcntl.h>
#include <cstdlib>

/* Detects platform characteristics. */
int main(int argc, char *argv[]) {
  int file = 0;
  char result[4096];
  if (argc != 2) {
    return 1;
  } else if ((file = open(argv[1], O_RDONLY)) < -1) {
    return 1;
  }
  realpath(argv[1], result);
  return 0;
}
