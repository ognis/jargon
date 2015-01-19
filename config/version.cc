/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#include <cstdio>

/* Defines architecture */
#if defined(__i386__)
#  define TRIPLET_ARCH "i386"
#elif defined(__x86_64__)
#  define TRIPLET_ARCH "x86_64"
#elif defined(__arm__)
#  define TRIPLET_ARCH "arm"
#else
#  define TRIPLET_ARCH "unknown"
#endif

/* Defines OS */
#if defined (__linux__)
#  define TRIPLET_OS "linux"
#elif defined (__FreeBSD__) || defined (__FreeBSD_kernel__)
#  define TRIPLET_OS "kfreebsd"
#elif !defined (__GNU__)
#  define TRIPLET_OS "unknown"
#endif
#ifdef __GNU__
#  define TRIPLET TRIPLET_ARCH "-" TRIPLET_ABI
#else
#  define TRIPLET TRIPLET_ARCH "-" TRIPLET_OS "-" TRIPLET_ABI
#endif

/* Defines calling convention and ABI */
#if defined (__ARM_EABI__)
#  if defined (__ARM_PCS_VFP)
#    define TRIPLET_ABI "gnueabihf"
#  else
#    define TRIPLET_ABI "gnueabi"
#  endif
#else
#  define TRIPLET_ABI "gnu"
#endif

/* Detects platform characteristics. */
int main(int argc, char *argv[]) {
  switch (argc == 2 ? argv[1][0] : 0) {
#ifdef __GNUC__
  case 'm':
    printf("%d\n", __GNUC_MINOR__);
    break;
  case 'v':
    printf("%d\n", __GNUC__);
    break;
#else
  case 'm':
  case 'v':
    puts("0");
    break;
#endif
  case 't':
    puts(TRIPLET);
    break;
  case -1:
    /* to test -Wno-unused-result */
    fread(NULL, 1, 1, NULL);
    break;
  }
  return 0;
}

