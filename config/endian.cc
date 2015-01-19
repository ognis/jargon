/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#include <cstdio>

/* Detects platform characteristics. */
int main(int argc, char *argv[]) {
  switch (argc == 2 ? argv[1][0] : 0) {
  case 'b': {
      volatile unsigned foo = 0x01234567;
      puts(*(unsigned char*)&foo == 0x67 ? "no" : "yes");
      break;
  }
  case -1:
    /* to test -Wno-unused-result */
    fread(NULL, 1, 1, NULL);
    break;
  }
  return 0;
}
