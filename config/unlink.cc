/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#if defined (HAVE_WINDOWS_H)
#  include <windows.h>
#endif
#if defined (HAVE_SYS_STAT_H)
#  include <sys/stat.h>
#endif
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

/* Detects platform characteristics. */
int main(int argc, char *argv[]) {
  int file = 0;
  if (argc != 2) {
    return 1;
  } else if ((file = open(argv[1], O_RDONLY)) < -1) {
    return 1;
  }
  CHECKING_UNLINK_FUNC(argv[1]);
  return 0;
}
