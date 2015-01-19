/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#if defined (CHECK_IF_WINDOWS_H_EXIST)
#  include <windows.h>
#elif defined (CHECK_IF_PTHREAD_H_EXIST)
#  include <pthread.h>
#elif (CHECK_IF_STDINT_H_EXIST)
#  include <cstdint>
#elif defined (CHECK_IF_STRING_H_EXIST)
#  include <string>
#elif defined (CHECK_IF_MEMORY_H_EXIST)
#  include <memory>
#elif defined (CHECK_IF_UNISTD_H_EXIST)
#  include <unistd.h>
#elif defined (CHECK_IF_STDIO_H_EXIST)
#  include <cstdio>
#elif defined (CHECK_IF_SYS_DIR_H_EXIST)
#  include <sys/dir.h>
#elif defined (CHECK_IF_SYS_NDIR_H_EXIST)
#  include <sys/ndir.h>
#elif defined (CHECK_IF_SYS_TYPES_H_EXIST)
#  include <sys/types.h>
#elif defined (CHECK_IF_ERRNO_H_EXIST)
#  include <cerrno>
#elif defined (CHECK_IF_WCHAR_H_EXIST)
#  include <cwchar>
#elif defined (CHECK_IF_WCTYPE_H_EXIST)
#  include <cwtype>
#elif defined (CHECK_IF_CTYPE_H_EXIST)
#  include <cctype>
#elif defined (CHECK_IF_DLFCN_H_EXIST)
#  include <dlfcn.h>
#elif defined (CHECK_IF_FCNTL_H_EXIST)
#  include <fcntl.h>
#elif defined (CHECK_IF_EXT_HASH_MAP_H_EXIST)
#  include <ext/hash_map>
#elif defined (CHECK_IF_EXT_HASH_SET_H_EXIST)
#  include <ext/hash_set>
#elif defined (CHECK_IF_HASH_MAP_H_EXIST)
#  include <hash_map>
#elif defined (CHECK_IF_HASH_SET_H_EXIST)
#  include <hash_set>
#elif defined (CHECK_IF_NDIR_H_EXIST)
#  include <ndir.h>
#elif defined (CHECK_IF_SYS_STAT_H_EXIST)
#  include <sys/stat.h>
#elif defined (CHECK_IF_SYS_TIMEB_H_EXIST)
#  include <sys/timeb.h>
#elif defined (CHECK_IF_SYS_TIME_H_EXIST)
#  include <sys/time.h>
#elif defined (CHECK_IF_SYS_MMAN_H_EXIST)
#  include <sys/mman.h>
#elif defined (CHECK_IF_GLOB_H_EXIST)
#  include <glob.h>
#elif defined (CHECK_IF_IO_H_EXIST)
#  ifdef __APPLE__
#    include <sys/uio.h>
#  else
#    include <sys/io.h>
#  endif
#elif defined (CHECK_IF_TCHAR_H_EXIST)
#  include <tchar.h>
#endif

/* Detects platform characteristics. */
int main(int argc, char *argv[]) {
  return 0;
}
