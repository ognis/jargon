/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#ifndef jargon_debug_error_h
#define jargon_debug_error_h

#include "../shared/common.h"

#define JARGON_ERR_UNKNOWN -1
#define JARGON_ERR_IO 1
#define JARGON_ERR_NULL_POINTER 2
#define JARGON_ERR_RUNTIME 3
#define JARGON_ERR_ILLEGAL_ARGUMENT 4
#define JARGON_ERR_INDEX_OUT_OF_BOUNDS 5
#define JARGON_ERR_NUMBER_FORMAT 6
#define JARGON_ERR_OUT_OF_MEMORY 7

#ifdef JARGON_DISABLE_NATIVE_EXCEPTION
#  include <csetjmp>
#else

/* Class represents exceptions. */
class JargonError {
  tchar_t* what;
  int error_number;
public:
  JargonError();
  JargonError(const JargonError& clone);
  JargonError(int error_number, const tchar_t* what, bool own_string);
  ~JargonError() throw();
  int number() const {return this->error_number;}
  tchar_t* explain();
  void set(int error_number, const tchar_t* what, bool own_string=false);
};

#define JARGON_TRY try
#define JARGON_CATCH_CLEAN_ELSE(error_number, cleanup_code, else_code) \
  catch (JargonError& error) { \
    if (error.number() != error_number) { \
      cleanup_code; \
      throw error; \
    } else { \
      else_code; \
    } \
  }
#define JARGON_CATCH_ELSE(error_number, else_code) \
  catch (JargonError& error) { \
    if (error.number() != error_number) { \
      throw error; \
    } else { \
      else_code; \
    } \
  }
#define JARGON_CATCH_CLEAN(error_number, cleanup_code) \
  catch (JargonError& error) { \
    if (error.number() != error_number) { \
      cleanup_code; \
      throw error; \
    } \
  }
#define JARGON_FINALLY(finally_code) \
  catch (...) { \
    finally_code; \
    throw; \
  } finally_code
#define JARGON_THROW(error_number, what) throw JargonError(error_number, what, false)
#define JARGON_THROW_DELETABLE(error_number, what) throw JargonError(error_number, what, true)

#endif /* JARGON_DISABLE_NATIVE_EXCEPTION */

#endif /* jargon_debug_error_h */
