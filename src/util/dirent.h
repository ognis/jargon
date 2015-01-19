/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#ifndef jargon_util_dirent_h
#define jargon_util_dirent_h

#if !defined(JARGON_HAVE_DIRENT_H) \
    && !defined(JARGON_HAVE_SYS_NDIR_H) \
    && !defined(JARGON_HAVE_SYS_DIR_H) \
    && !defined(JARGON_HAVE_NDIR_H)
#  ifdef _WIN64
typedef __int64 tintptr_t;
#  elif _WIN32
typedef int tintptr_t;
#  else
typedef int* tintptr_t;
#  endif
#  ifdef __APPLE__
#    include <sys/uio.h>
typedef struct _finddata_t {
  char* name;
} _finddeta_t;
#  else
#    include <sys/io.h>
#  endif
#  include "../shared/monolithic.h"

/** Structure dirent structure used by the dirent.h directory iteration functions. */
struct dirent {
  unsigned short d_namlen;
  char *d_name;
};

/** Structure tdir_t used by the dirent.h directory iteration functions. */
struct tdir_t {
  struct _finddata_t dd_dta;
  struct dirent	dd_dir;
  tintptr_t dd_handle;
  int32_t dd_stat;
  char dd_name[JARGON_MAX_PATH];
};

#define DIRENT_SEARCH_SUFFIX "*"
#define DIRENT_SLASH JARGON_PATH_DELIMITER_A

/* Returns a pointer to a tdir_t structure appropriately filled in to begin
   searching a directory. */
tdir_t* opendir(const char* directory);

/* Return a pointer to a dirent structure filled with the information on the
   next entry in the directory. */
struct dirent* readdir(tdir_t* directory);

/* Frees up resources allocated by opendir. */
int32_t closedir(tdir_t* directory);

#elif defined (JARGON_HAVE_DIRENT_H)
#  include <dirent.h>
#  define NAME_LENGTH(dirent) strlen((dirent)->d_name)
#  define tdir_t DIR
#else
#  define dirent direct
#  define NAM_LENGTH(dirent) (dirent)->d_namelen
#  define tdir_t DIR
#  if defined(JARGON_HAVE_SYS_NDIR_H)
#    include <sys/ndir.h>
#  endif
#  if defined(JARGON_HAVE_SYS_DIR_H)
#    include <sys/dir.h>
#  endif
#  if defined(JARGON_HAVE_NDIR_H)
#    include <ndir.h>
#  endif
#endif /* JARGON_HAVE_DIRENT_H */

#endif /* jargon_util_dirent_h */
