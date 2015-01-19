/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#ifndef jargon_core_constants_h
#define jargon_core_constants_h

/* Disables namespace macros. */
//#define JARGON_DONT_IMPLEMENT_NAMESPACE_MACROS

/* Disables hashmap/set usage. */
#ifndef JARGON_DISABLE_HASHING
#  define JARGON_DISABLE_HASHING
#endif

/* Define maximum path length. */
#define JARGON_MAX_PATH 4096

/* Define default path delimiter. */
#define JARGON_PATH_DELIMITER_A "/"

/* Define this to set an exact directory for the lock directory. */
//#define JARGON_LOCK_DIR "/tmp"

/* Define this to try and load the lock directory from this specified
   environment variable */
#define JARGON_LOCK_DIR_ENV_1 "TEMP"

/* Define this if you want to have look up this environment variable
   if the first one fails */
#define JARGON_LOCK_DIR_ENV_2 "TMP"

/* Define this if you want to have a fallback directory. */
#define JARGON_LOCK_DIR_ENV_FALLBACK "/tmp"

/* This must always be defined. They can be adjusted if required. */
#define JARGON_TERM_WILDCARD_STRING '*'
#define JARGON_TERM_WILDCARD_CHAR   '?'

/* Buffer size for input/output streams. Required. */
#define JARGON_STREAM_BUFFER_SIZE 1024

/* Buffer size for token streams. Required. */
#define JARGON_TOKEN_BUFFER_SIZE 32

/* Size of term-score cache. Required. */
#define JARGON_SCORE_CACHE_SIZE 32

/* Maximum length that the CharTokenizer uses. Required. */
#define JARGON_MAX_WORD_LENGTH 255

/* Maximum length of a token word. 
   Should be the same or more than JARGON_MAX_WORD_LENGTH */
#ifndef JARGON_OPTIMIZE_FOR_MEMORY
#  define JARGON_TOKEN_WORD_LENGTH JARGON_MAX_WORD_LENGTH
#endif

/* Maximum field length. */
#define JARGON_MAX_FIELD_LENGTH 100

/* The initial value set to BooleanQuery::maxClauseCount. */
#define JARGON_BOOLEANQUERY_MAX_CLAUSE_COUNT 1024

/* Maximum length of a term text. 
   Should be the same or more than JARGON_MAX_WORD_LEN */
#ifndef JARGON_OPTIMIZE_FOR_MEMORY
#  define JARGON_TERM_TEXT_LENGTH JARGONDE_MAX_WORD_LENGTH
#endif

/* Size of the CharTokenizer buffersize. Required. */
#define JARGON_IO_BUFFER_SIZE 1024

#endif /* jargon_core_constants_h */
