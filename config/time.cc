/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#if defined (HAVE_WINDOWS_H)
#  include <windows.h>
#endif
#include <sys/time.h>

/* Detects platform characteristics. */
int main(int argc, char *argv[]) {
  timeval now;
  gettimeofday(&now, NULL);
  return 0;
}
