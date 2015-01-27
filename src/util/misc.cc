/*
 * Copyright (C) 2014- Shingo OKAWA
 */
#include <assert.h>
#include <cctype>
#include <limits.h>
#include <map>
#include <iostream>
#include "dirent.h"
#include "misc.h"

#if defined(JARGON_HAVE_SYS_TIME_H)
#  include <sys/time.h>
#elif defined(JARGON_HAVE_TIME_H)
#  include <time.h>
#endif

#ifdef JARGON_HAVE_SYS_TIMEB_H
#  include <sys/timeb.h>
#endif

#if defined(JARGON_HAVE_SYS_STAT_H)
#  include <sys/stat.h>
#endif

#ifdef JARGON_HAVE_STRINGS_H
#  include <strings.h>
#endif

#ifdef JARGON_HAVE_UNISTD_H
#  include <unistd.h>
#endif

#ifdef JARGON_HAVE_FCNTL_H
#  include <fcntl.h>
#endif

#ifdef JARGON_HAVE_IO_H
#  ifdef __APPLE__
#    include <sys/uio.h>
#  else
#    include <sys/io.h>
#  endif
#endif


JARGON_NAMESPACE_BEGIN(util)

/* Returns current time in milli second. */
uint64_t misc::current_time_millis() {
#ifndef JARGON_HAVE_GETTIMEOFDAY_F
  struct _timeb tstruct;
  _ftime(&tstruct);
  return (((uint64_t) tstruct.time) * 1000) + tstruct.millitm;
#else
  struct timeval tstruct;
  if (gettimeofday(&tstruct, NULL) < 0) {
    return 0;
  }
  return (((uint64_t) tstruct.tv_sec) * 1000) + tstruct.tv_usec / 1000;
#endif
}

/* Replaces 'search' with 'replace' at every occurence of 'search' in 'value'. */
const tchar_t* misc::replace(
    const tchar_t* value,
    const tchar_t* search,
    const tchar_t* replace) {
  int32_t count = 0;
  size_t replace_length = tcslen(replace);
  size_t search_length = tcslen(search);
  size_t value_length = tcslen(value);
  const tchar_t* position = value;

  while ((position = tcsstr(position + 1, search)) != NULL) {
    ++count;
  }

  size_t new_length = (value_length - (search_length * count)) + (replace_length * count);
  tchar_t* result = JARGON_NEW_ARRAY(tchar_t, new_length + 1);
  result[new_length] = 0;
  if (count == 0) {
    tcscpy(result, value);
    return result;
  }

  tchar_t* current = result;
  const tchar_t* processing = value;
  position = value;
  while ((position = tcsstr(position + 1, search)) != NULL) {
    tcsncpy(current, processing, position - processing);
    current += (position - processing);
    processing = position;
    tcscpy(current, replace);
    current += replace_length;
    processing += search_length;
  }

  tcscpy(current, processing);
  return result;
}

/* Returns true if the given directory exist. */
bool misc::directory_exists(const char* path){
  if (!path || !*path) {
    return false;
  }
  tstat_t info;
  int32_t result = tstat(path, &info);
  return result == 0;
}

/* Returns the size of file. */
int64_t misc::file_size(const char* path) {
  tstat_t info;
  if (tstat(path, &info) == 0) {
    return info.st_size;
  } else {
    return -1;
  }
}

/* Returns the length of file. */
int64_t misc::file_length(int handle) {
  tstat_t info;
  if (tfstat(handle, &info) == -1) {
    return -1;
  }
  return info.st_size;
}

/* Unlinks the given file, waits until directory_exists is false. */
int misc::file_unlink(const char* path, int32_t attempts) {
  int32_t i;

  if (!path || !*path ) {
    return -1;
  }

  if (attempts == 0) {
    attempts = 1;
  }

  while (attempts != 0) {
    if(tunlink(path) != 0 ) {
      return -1;
    }

    i = 0;
    while (i < 100) {
      if (!misc::directory_exists(path)) {
	return 1;
      }

      if (++i > 50) {
	misc::sleep(1);
      }
    }

    if (attempts > 0) {
      attempts--;
    }
  }

  return 0;
}

/* Sleep until assigned milli seconds elepsed. */
void misc::sleep(const int ms) {
#if defined(JARGON_HAVE_USLEEP_F)
  usleep(ms * 1000);
#else
#  error no sleep function?
#endif
}

/* Lists all files in dir. */
bool misc::list_files(
    const char* directory_path,
    std::vector<std::string>& files,
    bool full_path) {
  tdir_t* directory = opendir(directory_path);
  if (directory == NULL) {
    return false;
  }

  struct dirent* current = readdir(directory);
  tstat_t info;
  std::string path;
  while (current != NULL) {
    path = std::string(directory->dd_name) + "/" + current->d_name;
    int32_t file_stat = tstat(path.c_str(), &info);
    if (file_stat == 0 && !(info.st_mode & S_IFDIR)) {
      if ((strcmp(current->d_name, ".")) && (strcmp(current->d_name, ".."))) {
        if (full_path) {
	  files.push_back(path);
	} else {
	  files.push_back(current->d_name);
	}
      }
    }
    current = readdir(directory);
  }

  closedir(directory);
  return true;
}

/* Returns true if the file is dot file. */
bool misc::is_dot_directory(const tchar_t* path) {
  if (path[0] == '\0') {
    return false;
  }
  if (path[0] == '.' && path[1] == '\0') {
    return true;
  }
  if (path[1] == '\0') {
    return false;
  }
  if (path[0] == '.' && path[1] == '.' && path[2] == '\0') {
    return true;
  }
  return false;
}

/* Generates hash code for the assigned single-string. */
size_t misc::c_hash_code(const char* value) {
  size_t hash_code = 0;
  while (*value != 0) {
    hash_code = hash_code * 31 + *value++;
  }
  return hash_code;
}

/* Generates hash code for the assigned single-string. */
size_t misc::c_hash_code(const char* value, size_t length){
  size_t hash_code = 0;
  for (size_t i = 0; i < length; i++) {
    hash_code = hash_code * 31 + *value++;
  }
  return hash_code;
}

/* Concatenates given strings. */
tchar_t* misc::t_concat(
    const tchar_t* a,
    const tchar_t* b,
    const tchar_t* c,
    const tchar_t* d,
    const tchar_t* e,
    const tchar_t* f) {
#define T_LENGTH(x) (x == NULL ? 0 : tcslen(x))
  const size_t total_length = T_LENGTH(a) + T_LENGTH(b) + T_LENGTH(c)
                              + T_LENGTH(d) + T_LENGTH(e) + T_LENGTH(f) + sizeof(tchar_t);
  tchar_t* buffer = JARGON_NEW_ARRAY(tchar_t, total_length);
  buffer[0]=0;
  if (a != NULL) {
    tcscat(buffer, a);
  }
  if (b != NULL) {
    tcscat(buffer, b);
  }
  if (c != NULL) {
    tcscat(buffer, c);
  }
  if (d != NULL) {
    tcscat(buffer, d);
  }
  if (e != NULL) {
    tcscat(buffer, e);
  }
  if (f != NULL) {
    tcscat(buffer, f);
  }
  return buffer;
}

/* Concatenates given single-strings. */
char* misc::c_concat(
    const char* a,
    const char* b,
    const char* c,
    const char* d,
    const char* e,
    const char* f ) {
#define C_LENGTH(x) (x == NULL ? 0 : strlen(x))
  const size_t total_length = C_LENGTH(a) + C_LENGTH(b) + C_LENGTH(c)
                              + C_LENGTH(d) + C_LENGTH(e) + C_LENGTH(f) + sizeof(char);
  char* buffer = JARGON_NEW_ARRAY(char, total_length);
  buffer[0] = 0;
  if (a != NULL) {
    strcat(buffer, a);
  }
  if (b != NULL) {
    strcat(buffer, b);
  }
  if (c != NULL) {
    strcat(buffer, c);
  }
  if (d != NULL) {
    strcat(buffer, d);
  }
  if (e != NULL) {
    strcat(buffer, e);
  }
  if (f != NULL) {
    strcat(buffer, f);
  }
  return buffer;
}

/* Creates a filename by concatenating segment with exteernal and x */
std::string misc::segment_name(
    const char* segment,
    const char* extension,
    const int32_t x) {
  JARGON_PRECOND(extension != NULL, "extension is NULL");
  if (x != -1) {
    char buffer[30];
    snprintf(buffer, 10, "%d", x);
    return std::string(segment) + extension + buffer;
  } else {
    return std::string(segment) + extension;
  }
}

/* Creates a filename in buffer by concatenating segment with external and x */
void misc::segment_name(
    char* buffer,
    int32_t length,
    const char* segment,
    const char* extension,
    const int32_t x) {
  JARGON_PRECOND(buffer != NULL, "buffer is NULL");
  JARGON_PRECOND(segment != NULL, "segment is NULL");
  JARGON_PRECOND(extension != NULL, "extension is NULL");
  if (x == -1) {
    snprintf(buffer, length, "%s%s", segment, extension);
  } else {
    snprintf(buffer, length, "%s%s%d", segment, extension, x);
  }
}

/* Compares two strings, character by character, and returns the
   first position where the two strings differ from one another. */
int32_t misc::t_compare(
    const tchar_t* s1,
    const int32_t s1_length,
    const tchar_t* s2,
    const int32_t s2_length) {
  int32_t length = s1_length < s2_length ? s1_length : s2_length;
  for (int32_t i = 0; i < length; i++) {
    if (s1[i] != s2[i]) {
      return i;
    }
  }
  return length;
}

/* In-place trimming for strings and words. */
tchar_t* misc::trim_string(tchar_t* value) {
  size_t i;
  size_t j;
  size_t length = tcslen(value);

  for (i = 0; i < length; i++) {
    if (!istspace(value[i])) {
      break;
    }
  }

  for (j = length - 1; j > i; --j) {
    if (!istspace(value[j])) {
      break;
    }
  }

  if (i == 0 && j == length - 1) {
    return value;
  }

  if (i == 0) {
    value[j + 1] = 0;
  } else {
    j++;
    tcsncpy(value, value + i, j - i);
    value[j - i] = 0;
  }

  return value;
}

tchar_t* misc::trim_word(tchar_t* value) {
  size_t i;
  size_t j;
  size_t length = tcslen(value);

  for (i = 0; i < length; i++) {
    if (!istspace(value[i]))
      break;
  }

  for (j = i; j < length; j++) {
    if (istspace(value[j])) {
      break;
    }
  }

  if (i == 0 && j == length) {
    return value;
  }

  if (i == j) {
    return NULL;
  }

  if (i == 0) {
    value[j] = 0;
    return value;
  } else {
    tcsncpy(value, value + i, j - i);
    value[j - i] = 0;
  }

  return value;
}

/* Casts long value to base36, and vice versa. */
size_t misc::long_to_base36(int64_t value, int32_t base, char* to) {
  static char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
  char buffer[(sizeof(unsigned long) << 3) + 1];
  char *start;
  char *end;

  start = end = buffer + sizeof(buffer) - 1;
  *start = '\0';
  do {
    *--start = digits[value % base];
    value /= base;
  } while (start > buffer && value);

  memcpy( to, start, end - start );
  to[end - start] = 0;
  return end - start;
}

int64_t misc::base36_to_long(const char* value) {
  char *start = (char*)value;
  int64_t result = 0;

  while ( *start != '\0' ) {
    result = isdigit(*start)
      ? (36 * result) + (*start - '0')
      : (36 * result) + (*start - 'a' + 10);
    start++;
  }

  return result;
}

/* Converts string. */
std::string misc::to_string(const int32_t value) {
  char buffer[20];
  tchar_t t_buffer[20];
  i64tot(value, t_buffer, 10);
  STRCPY_TtoA(buffer, t_buffer, 20);
  return buffer;
}

std::string misc::to_string(const int64_t value) {
  char buffer[20];
  tchar_t t_buffer[20];
  i64tot(value, t_buffer, 10);
  STRCPY_TtoA(buffer, t_buffer, 20);
  return buffer;
}

std::string misc::to_string(JARGON_THREAD_ID_T value) {
  static int32_t next_index = 0;
  static std::map<JARGON_THREAD_ID_T, int32_t> ids;
  if (ids.find(value) == ids.end()) {
    ids[value] = next_index++;
  }
  return misc::to_string(ids[value]);
}

std::string misc::to_string(const float_t value){
  char buffer[20];
  snprintf(buffer, 20, "%0.2f", (double)value);
  return buffer;
}

std::string misc::to_string(const bool value){
  return value ? "true" : "false";
}

std::string misc::to_string(const tchar_t* value, int32_t length){
  if (value == NULL || length == 0) {
    return "";
  }
  if (length < 0) {
    length = tcslen(value);
  }
  char* buffer = JARGON_NEW_ARRAY(char, length + 1);
  STRCPY_WtoA(buffer, value, length + 1);
  std::string result = buffer;
  JARGON_DEL_ARRAY(buffer);
  return result;
}

#ifndef JARGON_ASCII_MODE
size_t misc::w_hash_code(const wchar_t* value) {
  size_t hash_code = 0;
  while (*value != 0 ) {
    hash_code = hash_code * 31 + *value++;
  }
  return hash_code;
}

size_t misc::w_hash_code(const wchar_t* value, size_t length) {
  size_t hash_code = 0;
  for (size_t i = 0; i < length; i++) {
    hash_code = hash_code * 31 + *value++;
  }
  return hash_code;
}

char* misc::wide_to_char(const wchar_t* value) {
   size_t length = tcslen(value);
   char* result = JARGON_NEW_ARRAY(char, length + 1);
   misc::copy_wide_to_char(value, result, length + 1);
   return result;
}

wchar_t* misc::char_to_wide(const char* value) {
   size_t length = strlen(value);
   wchar_t* result = JARGON_NEW_ARRAY(wchar_t, length + 1);
   misc::copy_char_to_wide(value, result, length + 1);
   return result;
}

void misc::copy_wide_to_char(const wchar_t* source, char* destination, size_t length) {
  size_t source_length = wcslen(source);
  for (uint32_t i = 0; i < length && i < source_length + 1; i++) {
    destination[i] = OUT_OF_RANGE_CHAR(source[i]);
  }
}

void misc::copy_char_to_wide(const char* source, wchar_t* destination, size_t length) {
  size_t source_length = strlen(source);
  for (uint32_t i = 0; i < length && i < source_length + 1; i++) {
    destination[i] = source[i];
  }
}
#endif /* JARGON_ASCII_MODE */

JARGON_NAMESPACE_END
