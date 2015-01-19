/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#ifndef jargon_util_stringbuffer_h
#define jargon_util_stringbuffer_h

#include "../shared/monolithic.h"

JARGON_NAMESPACE_BEGIN(util)

class StringBuffer {
 public:
  /* Constructor. */
  StringBuffer();

  /* Constructor. Allocates a buffer of length size + 1 */
  StringBuffer(const size_t size);

  /* Copy constructor. */
  StringBuffer(const tchar_t* value);

  /* Constructs a StringBuffer using another buffer. */
  StringBuffer(tchar_t* buffer, size_t max, const bool consume);

  /* Destructor */
  virtual ~StringBuffer();

  /* Clears buffer */
  void clear();

  /* Appends String. */
  void append_string(const tchar_t* value);

  /* Appends sub String with length given. */
  void append_string(const tchar_t* value, size_t length);

  /* Appends single character. */
  void append_char(const tchar_t value);

  /* Appends Integer. */
  void append_int(const int64_t value, const int32_t radix=10);

  /* Appends Float. */
  void append_float(const float_t value, const size_t digits);

  /* Appends Float. */
  void append_boost(const float_t boost);

  /* Appends Boolean. */
  void append_bool(const bool value);

  /* Prepends String.*/
  void prepend(const tchar_t* value);

  /* Prepends sub String with length given. */
  void prepend(const tchar_t* value, size_t length);

  /* Enable << operation. */
  StringBuffer& operator<<(const tchar_t* value) {
    this->append_string(value);
    return *this;
  }

  /* Enable << operation. */
  StringBuffer& operator<<(const int64_t value) {
    this->append_int(value);
    return *this;
  }

  /* Sets Character at position given. */
  void char_at(size_t position, const tchar_t value);

  /* Returns Character at position given. */
  tchar_t char_at(size_t position);

  /* Inserts Character at position given. */
  void insert(const size_t position, tchar_t value);

  /* Inserts String at position given. */
  void insert(const size_t position, const tchar_t* value, size_t length=-1);

  /* Delets Character at position given. */
  void remove(size_t position);

  /* Delets String within range given. */
  void remove(size_t start, size_t end);

  /* Compares String between start and end with value given. */
  bool equals(size_t start, size_t end, const tchar_t* value, size_t length=-1) const;

  /* String length. */
  size_t length;

  /* Returns length. */
  size_t get_length() const;

  /* Returns platform-specific representation. */
  tchar_t* to_string();

  /* Transforms into lower case. */
  void to_lower();

  /* Returns null-terminated reference. */
  tchar_t* get_buffer();

  /* Returns null-terminated reference. */
  tchar_t* get_buffer(const bool release);

  /* Reverses buffer. */
  void reserve(const size_t length);

 private:
  /* Has the buffer grown to sufice the specified minimu length. */
  void grow(const size_t min, const size_t skip=0);
  tchar_t* buffer;
  size_t buffer_length;
  bool buffer_owned;
};

JARGON_NAMESPACE_END

#endif /* jargon_util_stringbuffer_h */
