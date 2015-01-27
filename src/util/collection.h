/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#ifndef jargon_util_collection_h
#define jargon_util_collection_h

#if defined(JARGON_HAVE_UNORDERED_MAP_H)
#  include <unordered_map>
#  include <unordered_set>
#  define JARGON_HASH_MAP unordered_map
#  define JARGON_HASH_SET unordered_set
#elif defined(JARGON_HAVE_HASH_MAP_H) && defined(JARGON_HAVE_HASH_SET_H)
#  include <hash_map>
#  include <hash_set>
#  define JARGON_HASH_MAP hash_map
#  define JARGON_HASH_SET hash_set
#elif defined(JARGON_HAVE_EXT_HASH_MAP_H) && defined(JARGON_HAVE_EXT_HASH_SET_H)
#  include <ext/hash_map>
#  include <ext/hash_set>
#  define JARGON_HASH_MAP hash_map
#  define JARGON_HASH_SET hash_set
#elif !defined(JARGON_DISABLE_HASHING)
#  define JARGON_DISABLE_HASHING
#  include <map>
#  include <set>
#endif

#endif /* jargon_util_collection_h */
