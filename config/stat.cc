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

  struct CHECKING_STAT_TYPE x;
  CHECKING_FSTAT_FUNC(file, &x);
  CHECKING_LSEEK_FUNC(file, 10, SEEK_SET);
  return 0;
}
