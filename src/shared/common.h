/*
 * Copyright (C) 2014- Shingo OKAWA
 */
#ifndef jargon_shared_common_h
#define jargon_shared_common_h

/* Platform includes that MUST be included for the public headers to work. */
#include "../configure.h"
#include "../constants.h"

/* Namespace helper. */
#if defined(JARGON_DONT_IMPLEMENT_NAMESPACE_MACROS)
// do nothing
#elif !defined(DISABLE_NAMESPACE) && defined(JARGON_HAVE_NAMESPACES)
#  define JARGON_NAMESPACE_BEGIN(sub) namespace jargon { namespace sub {
#  define JARGON_NAMESPACE_BEGIN2(sub, sub2) namespace jargon { namespace sub { namespace sub2 {
#  define JARGON_NAMESPACE_END }}
#  define JARGON_NAMESPACE_END2 }}}
#  define JARGON_NAMESPACE_USE(sub) using namespace jargon::sub;
#  define JARGON_NAMESPACE_USE2(sub, sub2) using namespace jargon::sub::sub2;
#  define JARGON_NAMESPACE(sub) jargon::sub
#  define JARGON_NAMESPACE2(sub, sub2) jargon::sub::sub2
#else
#  define JARGON_NAMESPACE_BEGIN(sub)
#  define JARGON_NAMESPACE_BEGIN2(sub, sub2)
#  define JARGON_NAMESPACE_END
#  define JARGON_NAMESPACE_END2
#  define JARGON_NAMESPACE_USE(sub)
#  define JARGON_NAMESPACE_USE2(sub, sub2)
#  define JARGON_NAMESPACE(sub)
#  define JARGON_NAMESPACE2(sub, sub2)
#endif

/* Macro for validating conditions. */
#ifndef JARGON_PRECOND
#  include <assert.h>
#  define JARGON_PRECOND(x,y) assert(x)
#endif

/* Creates new arrays. */
#define JARGON_NEW_ARRAY(type, size) (type*)calloc(size, sizeof(type))

/* Frees given objects. */
#define JARGON_FREE(x) {free(x); (x) = NULL;}
#define JARGON_XFREE(x) {free(x);}

/* Deletes array itself. */
#ifndef JARGON_DEL_ARRAY
#  define JARGON_DEL_ARRAY(x) {free(x); x = NULL;}
#  define JARGON_XDEL_ARRAY(x) {free(x);}
#endif

/* Deletes array and its elements. */
#define JARGON_DEL_ARRAY_ALL(x) \
  {if (x != NULL) {\
     for (int i = 0; x[i] != NULL; i++) {\
       JARGON_FREE(x[i]);\
     }\
     JARGON_DEL_ARRAY(x);\
  }}

/* Deletes array and its elements with specified deconstructor, if possible. */
#define JARGON_XDEL_ARRAY_ALL(x) \
  {if (x != NULL) {\
     for (int i = 0; x[i] != NULL; i++) {\
       JARGON_XFREE(x[i]);\
     }\
     JARGON_DEL_ARRAY(x);\
  }}

#endif /* jargon_shared_common_h */
