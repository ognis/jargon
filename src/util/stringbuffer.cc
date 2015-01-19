/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#include <cassert>
#include "stringbuffer.h"

JARGON_NAMESPACE_BEGIN(util)

/* Constructor. */
StringBuffer::StringBuffer() {
  this->buffer_length = JARGON_TOKEN_BUFFER_SIZE;
  this->length = 0;
  this->buffer = JARGON_NEW_ARRAY(tchar_t, this->buffer_length);
  this->buffer_owned = true;
}

/* Constructor. Allocates a buffer of length size + 1 */
StringBuffer::StringBuffer(const size_t size) {
  this->buffer_length = size + 1;
  this->length = 0;
  this->buffer = JARGON_NEW_ARRAY(tchar_t, this->buffer_length);
  this->buffer_owned = true;
}

/* Copy constructor. */
StringBuffer::StringBuffer(const tchar_t* value){
    this->length = (size_t) tcslen(value);
    const size_t occupied_size = this->length + 1;
    this->buffer_length = (occupied_size >= JARGON_TOKEN_BUFFER_SIZE ?
			   occupied_size
			   :
			   JARGON_TOKEN_BUFFER_SIZE);
    this->buffer = JARGON_NEW_ARRAY(tchar_t, this->buffer_length);
    this->buffer_owned  = true;
    tcsncpy(this->buffer, value, occupied_size);
    JARGON_PRECOND(this->buffer[this->length] == '\0', "Buffer was not correctly terminated");
  }

/* Constructs a StringBuffer using another buffer. */
StringBuffer::StringBuffer(tchar_t* buffer, size_t max, const bool consume) {
  this->buffer = buffer;
  this->buffer_length = max;
  this->buffer_owned = !consume;
  this->length = 0;
}

/* Deconstructor. */
StringBuffer::~StringBuffer() {
  if (this->buffer_owned) {
    JARGON_DEL_ARRAY(this->buffer);
  } else {
    this->buffer = NULL;
  }
}

/* Clears buffer */
void StringBuffer::clear(){
  JARGON_DEL_ARRAY(this->buffer);
  this->length = 0;
  this->buffer_length = JARGON_TOKEN_BUFFER_SIZE;
  this->buffer = JARGON_NEW_ARRAY(tchar_t, this->buffer_length);
}

/* Appends String. */
void StringBuffer::append_string(const tchar_t* value) {
  this->append_string(value, tcslen(value));
}

/* Appends sub String with length given. */
void StringBuffer::append_string(const tchar_t* value, size_t length) {
  if (this->length + length + 1 > this->buffer_length){
    grow(this->length + length + 1);
  }
  tcsncpy(this->buffer + this->length, value, length);
  this->length += length;
}

/* Appends single character. */
void StringBuffer::append_char(const tchar_t value) {
  if (this->length + 1 > this->buffer_length){
    this->grow(this->length + 1);
  }
  this->buffer[this->length] = value;
  this->length++;
}

/* Appends Integer. */
void StringBuffer::append_int(const int64_t value, const int32_t radix) {
  tchar_t buffer[30];
  i64tot(value, buffer, radix);
  this->append_string(buffer);
}

/* Appends Float. */
void StringBuffer::append_float(const float_t value, const size_t digits){
  assert(digits <= 8);
  tchar_t buffer[48];
  int64_t int64_value = (int64_t)value;
  i64tot(int64_value, buffer, 10);
  size_t length = 99 - tcslen(buffer);
  size_t required_size = length < (size_t)digits ? length : digits;

  if (required_size > 0){
    tcscat(buffer, TLITERAL("."));
    int64_t residual = (int64_t)((value - int64_value) * pow((float_t)10, (float_t)(required_size + 1)));
    if (residual < 0) {
      residual *= -1;
    }

    int64_t adjustment = residual / 10;
    if (residual - (adjustment * 10) >= 5) {
      adjustment++;
    }

    if (adjustment) {
      size_t zeros = required_size - (size_t)log10((float_t)adjustment) - 1;
      while(zeros-- > 0 && zeros < 10) {
	tcscat(buffer, TLITERAL("0"));
      }
    }

    i64tot(adjustment, buffer + tcslen(buffer), 10);
  }
  this->append_string(buffer);
}

/* Appends Boost. */
void StringBuffer::append_boost(const float_t boost){
  if (boost != 1.0f) {
    this->append_char(TLITERAL('^'));
    this->append_float(boost, 1);
  }
}

/* Appends Boolean. */
void StringBuffer::append_bool(const bool value) {
  this->append_string(value ? TLITERAL("true") : TLITERAL("false"));
}

/* Prepends String.*/
void StringBuffer::prepend(const tchar_t* value) {
  this->prepend(value, tcslen(value));
}

/* Prepends sub String with length given. */
void StringBuffer::prepend(const tchar_t* value, const size_t length) {
  if (this->length + length + 1 > this->buffer_length){
    this->grow(this->length + length + 1, length);
  }
  tcsncpy(this->buffer, value, length);
  this->length += length;
}

/* Sets Character at position given. */
void StringBuffer::char_at(size_t position, const tchar_t value) {
  JARGON_PRECOND(position < this->length, "position is not in string");
  this->buffer[position] = value;
}

/* Returns Character at position given. */
tchar_t StringBuffer::char_at(size_t position) {
  JARGON_PRECOND(position < this->length, "position is not in string");
  return this->buffer[position];
}

/* Inserts Character at position given. */
void StringBuffer::insert(const size_t position, tchar_t value) {
  JARGON_PRECOND(position <= this->length, "position is larger than string length");
  this->grow(this->length + 1, 0);
  memmove(&(this->buffer[position + 1]), &(this->buffer[position]), sizeof(tchar_t) * (this->length - position));
  this->buffer[position] = value;
  this->length++;
}

/* Inserts String at position given. */
void StringBuffer::insert(const size_t position, const tchar_t* value, size_t length) {
  JARGON_PRECOND(position <= this->length, "position is larger than string length");
  if (length == -1) {
    length = tcslen(value);
  }
  if (length > 0) {
    grow(this->length + length, 0);
    memmove(&(this->buffer[position + length]), &(this->buffer[position]), sizeof(tchar_t) * (this->length - position));
    memcpy(&(this->buffer[position]), value, sizeof(tchar_t) * (length));
    this->length += length;
  }
}

/* Delets Character at position given. */
void StringBuffer::remove(size_t position) {
  JARGON_PRECOND(position < this->length, "position is larger than string length");
  memmove(&(this->buffer[position]), &(this->buffer[position + 1]), sizeof(tchar_t) * (this->length - position));
  this->length--;
  this->buffer[this->length] = TLITERAL('\0');
}

/* Delets String within range given. */
void StringBuffer::remove(size_t start, size_t end) {
  JARGON_PRECOND(start <= end && end <= this->length, "start/end is not in string");
  if (start < end) {
    memmove(&(this->buffer[start]), &(this->buffer[end]), sizeof(tchar_t) * (this->length - end));
    this->buffer[this->length - (end - start)] = TLITERAL('\0');
    this->length -= end - start;
  }
}

/* Compares String between start and end with value given. */
bool StringBuffer::equals(size_t start, size_t end, const tchar_t* value, size_t length) const {
    if (length == -1) {
      length = tcslen(value);
    }

    if (end - start != length) {
      return false;
    }

    for (size_t c = start; c < end; c++) {
      if (this->buffer[c] != value[c - start]) {
        return false;
      }
    }
    return true;
  }

/* Returns length. */
size_t StringBuffer::get_length() const{
  return this->length;
}

/* Returns platform-specific representation. */
tchar_t* StringBuffer::to_string(){
  tchar_t* result = JARGON_NEW_ARRAY(tchar_t, this->length + 1);
  if (result) {
    tcsncpy(result, this->buffer, this->length);
    result[this->length] = '\0';
  }
  return result;
}

/* Transforms into lower case. */
void StringBuffer::to_lower() {
  tcslwr(buffer);
}

/* Returns null-terminated reference. */
tchar_t* StringBuffer::get_buffer() {
  if (this->length == this->buffer_length){
    grow(this->length + 1);
  }
  this->buffer[this->length] = '\0';
  return this->buffer;
}

/* Returns null-terminated reference. */
tchar_t* StringBuffer::get_buffer(const bool release) {
  tchar_t* result = this->get_buffer();
  if (release) {
    this->buffer = NULL;
    this->length = 0;
    this->buffer_length = 0;
    this->buffer_owned = false;
  }
  return result;
}

/* Reverses buffer. */
void StringBuffer::reserve(const size_t size){
  if (this->buffer_length >= size) {
    return;
  }

  this->buffer_length = size;
  tchar_t* buffer = JARGON_NEW_ARRAY(tchar_t, this->buffer_length);
  tcsncpy(buffer, this->buffer, this->length);
  buffer[this->length] = '\0';

  if (this->buffer) {
    JARGON_DEL_ARRAY(this->buffer);
  }
  this->buffer = buffer;
}

  /* Has the buffer grown to sufice the specified minimu length. */
void StringBuffer::grow(const size_t min, const size_t skip) {
  JARGON_PRECOND(min >= skip + this->length + 1,"skipping width is not large enough");
  if (!this->buffer_owned){
    assert(this->buffer_length >= min);
    return;
  }

  this->buffer_length *= 2;
  if (this->buffer_length < min){
    this->buffer_length = min;
  }

  tchar_t* buffer = JARGON_NEW_ARRAY(tchar_t, this->buffer_length);
  memset(buffer, 0, sizeof(tchar_t) * skip);
  tcsncpy(buffer + skip, this->buffer, this->length);
  buffer[skip + this->length] = '\0';
  JARGON_DEL_ARRAY(this->buffer);
  this->buffer = buffer;
}

JARGON_NAMESPACE_END
