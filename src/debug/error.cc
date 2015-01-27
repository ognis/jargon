/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#include "error.h"
#include "../util/misc.h"

#ifdef JARGON_DISABLE_NATIVE_EXCEPTION
#  warning "Jargon exceptions are disabled."
#else

/* Constructor. */
JargonError::JargonError() :
  what(NULL),
  error_number(0) {}

/* Constructor. */
JargonError::JargonError(
    int error_number,
    const tchar_t* what,
    const bool own_string) :
  error_number(error_number) {
  this->what = STRDUP_TtoT(what);
  if (own_string) {
    //    JARGON_DEL_ARRAY(what);
  }
}

/* Copy constructor. */
JargonError::JargonError(const JargonError& clone) {
  this->error_number = clone.error_number;
  this->what = NULL;
  if (clone.what != NULL) {
    this->what = STRDUP_TtoT(clone.what);
  }
}

/* Deconstructor. */
JargonError::~JargonError() throw() {
  JARGON_DEL_ARRAY(this->what);
}

/* Returns error explanation. */
tchar_t* JargonError::explain(){
  return this->what;
}

/* Sets error number and its explanation.*/
void JargonError::set(int error_number, const tchar_t* what, const bool own_string) {
  JARGON_DEL_ARRAY(this->what);
  this->what=STRDUP_TtoT(what);
  this->error_number = error_number;
  if (own_string) {
    //    JARGON_DEL_ARRAY(what);
  }
}

#endif /* JARGON_DISABLE_NATIVE_EXCEPTION */
