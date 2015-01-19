/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#ifndef jargon_util_misc_h
#define jargon_util_misc_h

#include <string.h>
#include <vector>
#include "threading.h"
#include "../shared/monolithic.h"

JARGON_NAMESPACE_BEGIN(util)

class misc {
 public:
  /* Returns current time in milli second. */
  static uint64_t current_time_millis();

  /* Replaces 'search' with 'replace' at every occurence of 'search' in 'value'. */
  static const tchar_t* replace(
      const tchar_t* value,
      const tchar_t* search,
      const tchar_t* replace);

  /* Returns true if the given directory exist. */
  static bool directory_exists(const char* path);

  /* Returns the size of file. */
  static int64_t file_size(const char* path);

  /* Returns the length of file. */
  static int64_t file_length(int handle);

  /* Unlinks the given file, waits until dir_exists is false. */
  static int32_t file_unlink(const char* path, int32_t attempts = -1);

  /* Sleep until assigned milli seconds elepsed. */
  static void sleep(const int ms);

  /* Lists all files in dir. */
  static bool list_files(
      const char* path,
      std::vector<std::string>& files,
      bool full_path=false);

  /* Returns true if the file is dot file. */
  static bool is_dot_directory(const tchar_t* path);

  /* Generates hash code for the assigned single-string. */
  static size_t c_hash_code(const char* value);

  /* Generates hash code for the assigned single-string. */
  static size_t c_hash_code(const char* value, size_t length);

  /* Concatenates given strings. */
  static tchar_t* t_concat(
      const tchar_t* a,
      const tchar_t* b,
      const tchar_t* c=NULL,
      const tchar_t* d=NULL,
      const tchar_t* e=NULL,
      const tchar_t* f=NULL);

  /* Concatenates given single-strings. */
  static char* c_concat(
      const char* a,
      const char* b,
      const char* c=NULL,
      const char* d=NULL,
      const char* e=NULL,
      const char* f=NULL);

  /* Creates a filename by concatenating segment with exteernal and x */
  static std::string segment_name(
      const char* segment,
      const char* extension,
      const int32_t x=-1);

  /* Creates a filename in buffer by concatenating segment with external and x */
  static void segment_name(
      char* buffer,
      int32_t length,
      const char* segment,
      const char* extension,
      const int32_t x=-1);

  /* Compares two strings, character by character, and returns the
     first position where the two strings differ from one another. */
  static int32_t t_compare(
      const tchar_t* s1,
      const int32_t s1_length,
      const tchar_t* s2,
      const int32_t s2_length);

  /* In-place trimming for strings and words. */
  static tchar_t* trim_string(tchar_t* value);
  static tchar_t* trim_word(tchar_t* value);

  /* Casts long value to base36, and vice versa. */
  static size_t long_to_base36(int64_t value, int32_t base, char* to);
  static int64_t base36_to_long(const char* value);

  /* Converts string. */
  static std::string to_string(const int32_t value);
  static std::string to_string(const int64_t value);
  static std::string to_string(const JARGON_THREAD_ID_T value);
  static std::string to_string(const float_t value);
  static std::string to_string(const bool value);
  static std::string to_string(const tchar_t* value, int32_t length=-1);

#ifndef JARGON_ASCII_MODE
  static size_t w_hash_code(const wchar_t* value);
  static size_t w_hash_code(const wchar_t* value, size_t length);
  static char* wide_to_char(const wchar_t* value);
  static wchar_t* char_to_wide(const char* value);
  static void copy_char_to_wide(const char* source, wchar_t* destination, size_t length);
  static void copy_wide_to_char(const wchar_t* source, char* destination, size_t length);
#  define t_hash_code w_hash_code
#else
#  define t_hash_code a_hash_code
#endif
};

JARGON_NAMESPACE_END

#endif
