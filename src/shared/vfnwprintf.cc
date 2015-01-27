/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#include <climits>
#include "wchar.h"
#include "../util/stringbuffer.h"

JARGON_NAMESPACE_BEGIN(shared)
JARGON_NAMESPACE_USE(util)

void _vfnwprintf(
    StringBuffer* buffer,
    size_t /*count*/,
    const wchar_t* format,
    va_list& valist) {
  const wchar_t *current = format;
  StringBuffer* builder = NULL;

  if (buffer == NULL) {
    builder = new StringBuffer;
  } else {
    builder = buffer;
  }

  while (*current) {
    while (*current && *current != '%') {
      builder->append_char(*current++);
    }

    if (*current == '%') {
      if (current[1] == '%') {
	builder->append_char('%');
	current += 2;
	continue;
      }

      current++;
      switch (*current) {
      case 's': {
	const tchar_t *argument = va_arg(valist, tchar_t*);
	if (!argument) {
	  argument = TLITERAL("(null)");
	}
	builder->append_string(argument);
	current++;
	break;
      }
      case 'c':
	builder->append_char((tchar_t)va_arg(valist, int));
	current++;
	break;
      default: {
	if (*current == 'p') {
	  builder->append_int((int32_t)va_arg(valist, long));
	} else {
	  if (*current == 'a' || *current == 'A' ||
	      *current == 'e' || *current == 'E' ||
	      *current == 'f' || *current == 'F' || 
	      *current == 'g' || *current == 'G') {
	    builder->append_float((float_t)va_arg(valist, double), 8);
	  } else if (*current == 'd' || *current == 'i' ) {
	    builder->append_int((int32_t)va_arg(valist, int));
	  } else if (*current == 'l' ){
	    tchar_t translated[100];
	    i64tot((int64_t)va_arg(valist, int64_t), translated, 10);
	    builder->append_string(translated);
	  }
	}
	current++;
	break;
      }
      }
    }
  }
	
  if (buffer == NULL) {
#ifndef JARGON_ASCII_MODE
    tchar_t* convertee = builder->get_buffer();
    char converted[MB_LEN_MAX];
    size_t value;
    size_t length = builder->get_length();
    for (size_t i = 0; i < length; i++) {
      value = wctomb(converted, *convertee);
      if (value > 0) {
	converted[value]='\0';
	fputs(converted, stdout);
      }
      convertee++;
    }
#else
    fputs(builder->get_buffer(), stdout);
#endif
    JARGON_FREE(builder);
  }
}

JARGON_NAMESPACE_END

