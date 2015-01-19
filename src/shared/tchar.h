/*
 * Copyright (C) 2014- Shingo OKAWA
 */
#ifndef jargon_shared_tchar_h
#define jargon_shared_tchar_h

#include "common.h"

#if defined (JARGON_ASCII_MODE)
#  define TLITERAL(x) x
#else
#  define TLITERAL(x) L ## x
#endif

#define TCSTOI64_RADIX(x, r) \
  ((x >= TLITERAL('0') && x <= TLITERAL('9')) ? \
   x - TLITERAL('0') \
   : \
   ((x >= TLITERAL('a') && x <= TLITERAL('z')) ? \
    x - TLITERAL('a') + 10 \
    : \
    ((x >= TLITERAL('A') && x <= TLITERAL('Z')) ? \
     x - TLITERAL('A') + 10 \
     : 1000)))

#define UTF8_PREPARE(target, mask, length) \
  do { \
    if (target < 128) { \
      length = 1; \
      mask = 0x7f; \
    } else if ((target & 0xe0) == 0xc0) { \
      length = 2; \
      mask = 0x1f; \
    } else if ((target & 0xf0) == 0xe0) { \
      length = 3; \
      mask = 0x0f; \
    } else if ((target & 0xf8) == 0xf0) { \
      length = 4; \
      mask = 0x07; \
    } else if ((target & 0xfc) == 0xf8) { \
      length = 5; \
      mask = 0x03; \
    } else if ((target & 0xfe) == 0xfc) { \
      length = 6; \
      mask = 0x01; \
    } else { \
      length = -1; \
    } \
  } while (0)

#define TO_UTF8(target, source, count, mask, length) \
  (target) = (source)[0] & (mask); \
  for ((count) = 1; (count) < (length); ++(count)) { \
    if (((source)[(count)] & 0xc0) != 0x80) { \
      (target) = -1; \
      break; \
    } \
    (target) <<= 6; \
    (target) |= ((source)[(count)] & 0x3f); \
  }

/* If a wide character is being converted to a ascii character and it
   cannot fit, this character is used instead. */
#define OUT_OF_RANGE_CHAR(c) ((char)(((unsigned short)c)&0xFF))

#ifndef JARGON_HAVE_TCHAR_H
#  ifndef JARGON_ASCII_MODE
/* Make a formatted a string */
#    define sntprintf swprintf
/* Print a formatted string */
#    define tprintf wprintf
/* Print a formatted string using variable arguments */
#    define vsntprintf vsnwprintf
/* Check alpha/numeric char */
#    define istalnum iswalnum
/* Check alpha char */
#    define istalpha iswalpha
/* Check space char */
#    define istspace iswspace
/* Check digit char */
#    define istdigit iswdigit
/* Convert char to lower case */
#    define totlower towlower
/* Convert char to lower case */
#    define totupper towupper
/* convert string to lower case */
#    define tcslwr wcslwr
/* Copy a string to another string */
#    define tcscpy wcscpy
/* Copy a specified amount of one string to another string. */
#    define tcsncpy wcsncpy
/* Copy a string onto the end of the other string */
#    define tcscat wcscat
/* Copy a string onto the end of the other string */
#    define tcsncat wcsncat
/* Find location of one character */
#    define tcschr wcschr
/* Find location of a string */
#    define tcsstr wcsstr
/* Get length of a string */
#    define tcslen wcslen
/* Case sensitive compare two strings */
#    define tcscmp wcscmp
/* Case sensitive compare two strings */
#    define tcsncmp wcsncmp
/* Location of any of a set of character in a string */
#    define tcscspn wcscspn
/* Case insensitive compare two string */
#    define tcsicmp wcscasecmp
/* Duplicate strings. */
/*#    define tcsdup JARGON_NAMESPACE(shared)::wcsdup*/
#    define tcsdup wcsdup
/* Convert a string to a double */
#    define tcstod wcstod
/* Convert a string to an 64bit bit integer */
#    define tcstoi64 wcstoll
/* Convert 64bit bit integer to a string. */
#    define itot i64tot
/* Convert a 64 bit integer to a string (with base) */
/*#    define i64tot JARGON_NAMESPACE(shared)::i64tot //lltow */
#    define i64tot lltow 
#  else /* if defined(JARGON_ASCII_MODE) */
#    define sntprintf snprintf
#    define tprintf printf
#    define vsntprintf vsnprintf
#    define istalnum isalnum
#    define istalpha isalpha
#    define istspace isspace
#    define istdigit isdigit
#    define totlower tolower
#    define totupper toupper
#    define tcslwr strlwr
#    define tcscpy strcpy
#    define tcsncpy strncpy
#    define tcscat strcat
#    define tcsncat strncat
#    define tcschr strchr
#    define tcsstr strstr
#    define tcslen strlen
#    define tcscmp strcmp
#    define tcsncmp strncmp
#    define tcsicmp strcasecmp
#    define tcscspn strcspn
#    define tcsdup strdup
#    define tcstod strtod
#    define tcstoi64 strtoll
#    define itot i64tot
/*#    define i64tot JARGON_NAMESPACE(shared)::i64tot //lltoa*/
#    define i64tot lltoa
#  endif
#else  /* JARGON_HAVE_TCHAR_H */
#  include <tchar.h>
/* some tchar headers miss these. */
#  ifndef tcstoi64
#    ifndef JARGON_ASCII_MODE
/* Convers a string to an 64bit bit integer */
#      define tcstoi64 wcstoll
#    else
#      define tcstoi64 strtoll
#    endif
#  endif
#endif /* JARGON_HAVE_TCHAR_H */

#ifndef ttoi
#  define ttoi(x) (int)tcstoi64(x, NULL, 10)
#endif

#ifndef itot
#  define itot(i, buf, radix) JARGON_NAMESPACE(shared)::i64tot(i, buf, radix)
#endif

namespace std {
#ifndef tstring
#  ifndef JARGON_ASCII_MODE
typedef std::wstring tstring;
#  else
typedef std::string tstring;
#  endif
#endif
};

#define STRCPY_AtoA(target, source, length) strncpy(target, source, length)
#define STRDUP_AtoA(x) strdup(x)
#ifndef JARGON_ASCII_MODE
#  define string_duplicate(x) tcsdup(x)
#  define STRDUP_WtoW jargon_wcsdup
#  define STRDUP_TtoT STRDUP_WtoW
#  define STRDUP_WtoT STRDUP_WtoW
#  define STRDUP_TtoW STRDUP_WtoW
#  define STRDUP_AtoW(x) JARGON_NAMESPACE(util)::misc::char_to_wide(x)
#  define STRDUP_AtoT STRDUP_AtoW
#  define STRDUP_WtoA(x) JARGON_NAMESPACE(util)::misc::wide_to_char(x)
#  define STRDUP_TtoA STRDUP_WtoA
#  define STRCPY_WtoW(target, source, length) tcsncpy(target, source, length)
#  define STRCPY_TtoW STRCPY_WtoW
#  define STRCPY_WtoT STRCPY_WtoW
#  define STRCPY_AtoW(target, source, length) \
JARGON_NAMESPACE(util)::misc::copy_char_to_wide(source, target, length)
#  define STRCPY_AtoT STRCPY_AtoW
#  define STRCPY_WtoA(target, source, length) \
JARGON_NAMESPACE(util)::misc::copy_wide_to_char(source, target, length)
#  define STRCPY_TtoA STRCPY_WtoA
#else
#  define string_duplicate(x) strdup(x)
#  define STRDUP_AtoT STRDUP_AtoA
#  define STRDUP_TtoA STRDUP_AtoA
#  define STRDUP_TtoT STRDUP_AtoA
#define STRDUP_WtoT(x) xxxxxxxxxxxxxxx
#  define STRCPY_WtoT(target, source, length) xxxxxxxxxxxxxxx
#  define STRCPY_AtoT STRCPY_AtoA
#  define STRCPY_TtoA STRCPY_AtoA
#endif

#endif /* jargon_shared_tchar_h */
