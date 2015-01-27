/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#ifndef jargon_util_algorithm_h
#define jargon_util_algorithm_h

#include <cstdlib>
#include "../shared/monolithic.h"
#include "object.h"

JARGON_NAMESPACE_BEGIN(util)

template<typename ValueType>
class algorithm {
 public:
  /* Merge sort templetized implementation. */
  static void msort(
      ValueType* array,
      int32_t length,
      int32_t from,
      int32_t to,
      int32_t chunk_size=4) {
    JARGON_PRECOND(from < to, "must start index is less than end.");
    JARGON_PRECOND(from >= 0, "must start index is greater than 0.");
    for (int32_t chunk = from; chunk < to; chunk += chunk_size) {
      int32_t end = JARGON_MIN(chunk + chunk_size, to);
      for (int32_t i = chunk + 1; i < end; i++) {
	if (compare(array[i - 1], array[i]) > 0){
	  int32_t j = i;
	  ValueType value = array[j];
	  do {
	    array[j] = array[j - 1];
	    j--;
	  } while (j > chunk && compare(array[j - 1], value) > 0);
	  array[j] = value;
	}
      }
    }

    int32_t total_size = to - from;
    if (total_size <= chunk_size) {
      return;
    }

    ValueType* source = array;
    ValueType* target = JARGON_NEW_ARRAY(ValueType, length);
    ValueType* swap_holder = NULL;
    int32_t difference = -from;

    for (int32_t size = chunk_size; size < total_size; size <<= 1) {
      for (int32_t start = from; start < to; start += size << 1) {
	int32_t middle = start + size;
	int32_t end = JARGON_MIN(to, middle + size);
	if (middle >= end || compare(source[middle - 1], source[middle]) <= 0) {
	  memcpy(target + start + difference,
		 source + start,
		 (end - start) * sizeof(ValueType));
	} else if (compare(source[start], source[end - 1]) > 0) {
	  memcpy(target + end - size + difference,
		 source + start,
		 size * sizeof(ValueType));
	  memcpy(target + start + difference,
		 source + middle,
		 (end - middle) * sizeof(ValueType));
	} else {
	  int32_t left = start;
	  int32_t right = middle;
	  int32_t i = start + difference;
	  while (left < middle && right < end) {
	    target[i++] = source[(compare(source[left], source[right]) <= 0
				  ? left++ : right++)];
	  }
	  if (left < middle) {
	    memcpy(target + i, source + left, (middle-left) * sizeof(ValueType));
	  } else {
	    memcpy(target + i, source + right, (end - right) * sizeof(ValueType));
	  }
	}
      }
      swap_holder = source;
      source = target;
      target = swap_holder;
      from += difference;
      to += difference;
      difference = -difference;
    }

    if (source != array) {
      memcpy(array + difference, source, to * sizeof(ValueType));
      JARGON_FREE(source);
    } else {
      JARGON_FREE(target);
    }
  }
};

JARGON_NAMESPACE_END

#endif /* jargon_util_algorithm_h */
