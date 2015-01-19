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
  const wchar_t *iterater = format;
  StringBuffer* builder = NULL;

  if (buffer == NULL) {
    builder = new StringBuffer;
  } else {
    builder = buffer;
  }

  while (*iterater) {
    while (*iterater && *iterater != '%') {
      builder->append_char(*iterater++);
    }

    if (*iterater == '%') {
      if (iterater[1] == '%') {
	builder->append_char('%');
	iterater += 2;
	continue;
      }

      iterater++;
      switch (*iterater) {
      case 's': {
	const tchar_t *argument = va_arg(valist, tchar_t*);
	if (!argument) {
	  argument = TLITERAL("(null)");
	}
	builder->append_string(argument);
	iterater++;
	break;
      }
      case 'c':
	builder->append_char((tchar_t)va_arg(valist, int));
	iterater++;
	break;
      default: {
	if (*iterater == 'p') {
	  builder->append_int((int32_t)va_arg(valist, long));
	} else {
	  if (*iterater == 'a' || *iterater == 'A' ||
	      *iterater == 'e' || *iterater == 'E' ||
	      *iterater == 'f' || *iterater == 'F' || 
	      *iterater == 'g' || *iterater == 'G') {
	    builder->append_float((float_t)va_arg(valist, double), 8);
	  } else if (*iterater == 'd' || *iterater == 'i' ) {
	    builder->append_int((int32_t)va_arg(valist, int));
	  } else if (*iterater == 'l' ){
	    tchar_t translated[100];
	    i64tot((int64_t)va_arg(valist, int64_t), translated, 10);
	    builder->append_string(translated);
	  }
	}
	iterater++;
	break;
      }
      }
    }
  }
	
  if (buffer == NULL) {
#ifndef JARGON_ASCII_MODE
    tchar_t* pointer = builder->get_buffer();
    char ob[MB_LEN_MAX];
    size_t value;
    size_t length = builder->get_length();
    for (size_t i = 0; i < length; i++) {
      value = wctomb(ob, *pointer);
      if ( value > 0 ) {
	ob[value]='\0';
	fputs(ob, stdout);
      }
      pointer++;
    }
#else
    fputs(builder->get_buffer(), stdout);
#endif
    JARGON_FREE(builder);
  }
}

JARGON_NAMESPACE_END

