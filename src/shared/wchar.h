/*
 * Copyright (C) 2014- Shingo OKAWA
 */
#ifndef jargon_shared_wchar_h
#define jargon_shared_wchar_h

#include <cstdarg>
#include "common.h"
#include "tchar.h"
#ifdef JARGON_HAVE_STRING_H
#  include <string.h>
#endif
#ifdef JARGON_HAVE_WCHAR_H
#  include <cwchar>
#endif

JARGON_NAMESPACE_BEGIN(shared)

/* Compares string case-foldingly. */
int _tcscasefoldcmp(const tchar_t* target, const tchar_t* source);

/* Make the given word into case-folding form. */
tchar_t* _tcscasefold(tchar_t* value, int length=-1);

/* Converts utf8 character into unicode. */
size_t _utf8towc(wchar_t& target, const char *source);

/* Converts utf8 strign into unicode. */
size_t _utf8towcs(wchar_t* target, const char* source, size_t max);

/* Converts unicode into utf8 character. */
size_t _wctoutf8(char* target, const wchar_t source);

/* Converts unicode into utf8 string. */
size_t _wcstoutf8(char* target, const wchar_t* source, size_t max);

/* Converts unicode into utf8 string without assignment. */
#ifdef JARGON_ASCII_MODE
#  define _wcstoutf8(value, length) value
#else
std::string _wcstoutf8(const wchar_t* value, size_t length);
#endif

/* Returns the number of characters that this first utf8 character will expect. */
size_t _utf8charlen(const unsigned char p);

/* Compares given two strings. */
#if (!defined(JARGON_ASCII_MODE) && !defined(JARGON_HAVE_WCSCASECMP_F)) \
    || (defined(JARGON_ASCII_MODE) && !defined(JARGON_HAVE_STRCASECMP_F))
int _tcscasecmp(const tchar_t* target, const tchar_t* source);
#  undef tcsicmp
#  define tcsicmp JARGON_NAMESPACE(shared)::_tcscasecmp
#endif

/* Converts lower case. */
#if (!defined(JARGON_ASCII_MODE) && !defined(JARGON_HAVE_WCSLWR_F)) \
    || (defined(JARGON_ASCII_MODE) && !defined(JARGON_HAVE_STRLWR_F))
tchar_t* _tcslwr(tchar_t* value);
#  undef tcslwr
#  define tcslwr JARGON_NAMESPACE(shared)::_tcslwr
#endif

/* Converts int64 to string. */
#if (defined(JARGON_ASCII_MODE) && !defined(JARGON_HAVE_LLTOA_F)) \
    || (!defined(JARGON_ASCII_MODE) && !defined(JARGON_HAVE_LLTOW_F))
tchar_t* _i64tot(int64_t value, tchar_t* str, int radix);
#  undef i64tot
#  define i64tot JARGON_NAMESPACE(shared)::_i64tot
#endif

/* Duplicates given string. */
#if !defined(JARGON_HAVE_WCSDUP_F)
wchar_t* _wcsdup(const wchar_t* value);
#endif

/* Converts string to int64. */
#if (!defined(JARGON_ASCII_MODE) && !defined(JARGON_HAVE_WCSTOLL_F)) \
    || (defined(JARGON_ASCII_MODE) && !defined(JARGON_HAVE_STRTOLL_F))
int64_t _tcstoi64(const tchar_t* value, tchar_t** end, int radix);
#  undef tcstoi64
#  define tcstoi64 JARGON_NAMESPACE(shared)::_tcstoi64
#endif

/* Converts string to doubel. */
#if !defined(JARGON_ASCII_MODE) && !defined(JARGON_HAVE_WCSTOD_F)
double _tcstod(const tchar_t* value, tchar_t** end);
#  undef tcstod
#  define tcstod JARGON_NAMESPACE(shared)::_tcstod
#endif

/* printf function. */
#if !defined(JARGON_ASCII_MODE) && !defined(JARGON_HAVE_SNWPRINTF_F)
int _snwprintf(wchar_t* buffer, size_t count, const wchar_t * format, ...);
#  undef sntprintf
#  define sntprintf JARGON_NAMESPACE(shared)::_snwprintf
#endif

/* printf function. */
#if !defined(JARGON_ASCII_MODE) && !defined(JARGON_HAVE_WPRINTF_F)
void _wprintf(const wchar_t * format, ...);
#  undef tprintf
#  define tprintf JARGON_NAMESPACE(shared)::_wprintf
#endif

/* printf function. */
#if !defined(JARGON_ASCII_MODE) && !defined(JARGON_HAVE_VSNWPRINTF_F)
int _vsnwprintf(
    wchar_t * buffer,
    size_t count,
    const wchar_t * format,
    va_list& ap);
#undef vsntprintf
#define vsntprintf JARGON_NAMESPACE(shared)::_vsnwprintf
#endif

JARGON_NAMESPACE_END

#endif /* jargon_shared_wchar_h */
