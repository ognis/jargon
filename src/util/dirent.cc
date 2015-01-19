/*
 * Copyright (C) 2014- Shingo OKAWA
 */
#if !defined(JARGON_HAVE_DIRENT_H) \
    && !defined(JARGON_HAVE_SYS_NDIR_H) \
    && !defined(JARGON_HAVE_SYS_DIR_H) \
    && !defined(JARGON_HAVE_NDIR_H)
#  include <glob.h>
#  include <sys/stat.h>
#  include <cerrno>
#  include <string>
#  include "dirent.h"

/* Handles glob function. */
typedef struct findhandle_t {
  glob_t glob_buffer;
  size_t position;
} findhandle_t;

/* Retrieve the next file descriptor within the current context. */
int tfindnext(int* int_handle, _finddata_t* info) {
  findhandle_t* handle = (findhandle_t*) int_handle;
  char* position;
  if (handle->position >= handle->glob_buffer.gl_pathc) {
    return -1;
  }
  char* last_slash = handle->glob_buffer.gl_pathv[handle->position];
  for (position = last_slash; *position != 0; position++) {
    if (*position == '/') {
      last_slash = position + 1;
    }
  }
  info->name = last_slash;
  handle->position++;
  return 0;
}

/* Retrieve the first file descriptor within the assigned direcory. */
tintptr_t tfindfirst(const char* directory, _finddata_t* info) {
  findhandle_t* handle = (findhandle_t*) malloc(sizeof(findhandle_t));
  memset(handle, 0, sizeof(findhandle_t));
  if (glob(directory, 0, 0, &handle->glob_buffer) != 0) {
    return (tintptr_t) -1;
  }
  if (tfindnext((int*) handle, info) < 0) {
    return (tintptr_t) -1;
  }
  return (tintptr_t) handle;
}

/* Free the glob context. */
void tfindclose(int* int_handle) {
  findhandle_t* handle = (findhandle_t*) int_handle;
  globfree(&handle->glob_buffer);
}

/* Returns a pointer to a tdir_t structure appropriately filled in to begin
   searching a directory. */
tdir_t* opendir(const char *directory) {
  tdir_t *nd;
  char full_path[JARGON_MAX_PATH];
  errno = 0;

  if (!directory) {
    errno = EFAULT;
    return NULL;
  }

  if (directory[0] == '\0') {
    errno = ENOTDIR;
    return NULL;
  }

  tstat_t info;
  if (tstat(directory, &info) == -1) {
    errno = ENOENT;
    return NULL;
  }

  if (!(info.st_mode & S_IFDIR)) {
    errno = ENOTDIR;
    return NULL;
  }

  trealpath(directory, full_path);
  tdir_t* new_directory = new tdir_t;
  if (!new_directory) {
    errno = ENOMEM;
    return NULL;
  }

  strcpy(new_directory->dd_name, full_path);
  if (new_directory->dd_name[0] != '\0'
      && new_directory->dd_name[strlen (new_directory->dd_name) - 1] != '/'
      && new_directory->dd_name[strlen (new_directory->dd_name) - 1] != '\\') {
    strcat(new_directory->dd_name, DIRENT_SLASH);
  }

  strcat (new_directory->dd_name, DIRENT_SEARCH_SUFFIX);
  new_directory->dd_handle = (tintptr_t) -1;
  new_directory->dd_stat = 0;
  new_directory->dd_dir.d_namlen = 0;
  new_directory->dd_dir.d_name = new_directory->dd_dta.name;

  return new_directory;
}

/* Return a pointer to a dirent structure filled with the information on the
   next entry in the directory. */
struct dirent* readdir(tdir_t * directory) {
  errno = 0;
  if (!directory) {
    errno = EFAULT;
    return NULL;
  }

  if (directory->dd_dir.d_name != directory->dd_dta.name) {
    errno = EINVAL;
    return NULL;
  }

  bool call_findnext = true;
  if (directory->dd_stat < 0) {
    return NULL;
  } else if (directory->dd_stat == 0) {
    directory->dd_handle = tfindfirst(directory->dd_name, &(directory->dd_dta));
    if (directory->dd_handle == (tintptr_t) -1) {
      directory->dd_stat = -1;
    } else {
      directory->dd_stat = 1;
    }
    call_findnext = false;
  }

  while (directory->dd_stat > 0) {
    if (call_findnext) {
      if (tfindnext (directory->dd_handle, &(directory->dd_dta))) {
	tfindclose (directory->dd_handle);
	directory->dd_handle = (tintptr_t) -1;
	directory->dd_stat = -1;                      
	return NULL;
      } else {
	directory->dd_stat++;
      }
    }

    directory->dd_dir.d_namlen = strlen(directory->dd_dir.d_name);
    bool this_directory_or_parent = directory->dd_dir.d_name[0] == '.'
                                    && (directory->dd_dir.d_name[1] == 0
					|| (directory->dd_dir.d_name[1] == '.'
					    && directory->dd_dir.d_name[2] == 0));

    if (!this_directory_or_parent) {
      tstat_t buffer;
      char path[JARGON_MAX_PATH];
      size_t length = strlen(directory->dd_name) - strlen(DIRENT_SEARCH_SUFFIX);
      strncpy(path, directory->dd_name, length);
      path[length] = 0;
      strcat(path, directory->dd_dir.d_name);     
      if (tstat(path, &buffer) == 0) {
	return &directory->dd_dir;
      }
    }

    call_findnext = true;
  }
	return NULL;
}

/* Frees up resources allocated by opendir. */
int32_t closedir(tdir_t * directory) {
  int32_t result = 0;
  errno = 0;

  if (!directory) {
    errno = EFAULT;
    return -1;
  }

  if (directory->dd_handle != (tintptr_t) -1) {
    //    result = tfindclose(directory->dd_handle);
    tfindclose(directory->dd_handle);
  }

  JARGON_FREE(directory);
  return result;
}

#endif /* JARGON_HAVE_DIRENT_H */
