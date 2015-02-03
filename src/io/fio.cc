/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#include "fio.h"
#include "../debug/error.h"
#include "../util/misc.h"

JARGON_NAMESPACE_BEGIN(io)
JARGON_NAMESPACE_USE(util)

/* Constructor. */
FileInput::FileInput() :
  NamedObject() {}

/* Deconstructor. */
FileInput::~FileInput() {}

/* Copy constructor. */
FileInput::FileInput(const FileInput& /*clone*/) {}

/* Reads four bytes and returns an int. */
int32_t FileInput::read_int() {
  int32_t bytes = (this->read_byte() << 24);
  bytes |= (this->read_byte() << 16);
  bytes |= (this->read_byte() <<  8);
  return (bytes | this->read_byte());
}

/* Reads an int stored in variable-length format. See VLQ. */
int32_t FileInput::read_variable_int() {
  uint8_t byte = this->read_byte();
  int32_t result = byte & 0x7F;
  // If the first bit is 1, then another VLQ octet follows.
  for (int32_t shift = 7; (byte & 0x80) != 0; shift += 7) {
    byte = this->read_byte();
    result |= (byte & 0x7F) << shift;
  }
  return result;
}

/* Reads eight bytes and returns a long. */
int64_t FileInput::read_long() {
  int64_t result = ((int64_t)this->read_int() << 32);
  return (result | ((int64_t)this->read_int() & 0xFFFFFFFFL));
}

/* Reads a long stored in variable-length format. See VLQ. */
int64_t FileInput::read_variable_long() {
  uint8_t byte = this->read_byte();
  int64_t result = byte & 0x7F;
  // If the first bit is 1, then another VLQ octet follows.
  for (ixnt32_t shift = 7; (byte & 0x80) != 0; shift += 7) {
    byte = this->read_byte();
    result |= (((int64_t)byte) & 0x7FL) << shift;
  }
  return result;
}

/* Skips assigned length characters. */
void FileInput::skip_chars(const int32_t length) {
  for (int32_t i = 0; i < length; i++) {
    tchar_t byte = this->read_byte();
    if ((byte & 0x80) == 0) {
      // Do Nothing.
    } else if ((byte & 0xE0) != 0xE0) {
      this->read_byte();
    } else {
      this->read_byte();
      this->read_byte();
    }
  }
}

/* Reads a string */
int32_t FileInput::read_string(tchar_t* buffer, const int32_t max) {
  int32_t length = this->read_variable_int();
  int32_t max_length = max - 1;
  if (length >= max_length) {
    this->read_chars(buffer, 0, max_length);
    buffer[max_length] = 0;
    if (length - max_length > 0) {
      this->skip_chars(length - max_length);
    }
    return max_length;
  } else {
    this->read_chars(buffer, 0, length);
    buffer[length] = 0;
    return length;
  }
}

/* Reads a string */
TCHAR* FileInput::read_string() {
  int32_t length = this->read_variable_int();
  if (length == 0) {
    return string_duplicate(JARGON_BLANK_STRING);
  }
  tchar_t* result = JARGON_NEW_ARRAY(tchar_t, length + 1);
  this->read_chars(result, 0, length);
  result[length] = 0;
  return result;
}

#ifndef JARGON_ASCII_MODE
/* Reads a string and converts to ascii. */
int32_t FileInput::read_string(char* buffer, const int32_t max) {
  tchar_t* tbuffer = JARGON_NEW_ARRAY(tchar_t, max);
  int32_t result = -1;
  JARGON_TRY {
    result = this->read_string(tbuffer, max);
    STRCPY_TtoA(buffer, tbuffer, result + 1);
  } JARGON_FINALLY (JARGON_DELETE_CARRAY(tbuffer));
  return result;
}
#endif

/* Reads a specified number of bytes into an array at the specified offset. */
void FileInput::read_bytes(uint8_t* buffer, const int32_t length, bool /*use_buffer*/) {
  this->read_bytes(buffer, length);
}

/* Reads UTF-8 encoded characters into an array. */
void FileInput::read_chars(tchar_t* buffer, const int32_t start, const int32_t length) {
  const int32_t end = start + length;
  tchar_t bytes;
  for (int32_t i = start; i < end; ++i) {
    bytes = this->read_byte();
    if ((bytes & 0x80) == 0) {
      bytes = (bytes & 0x7F);
    } else if ((bytes & 0xE0) != 0xE0) {
      bytes = (((bytes & 0x1F) << 6) | (this->read_byte() & 0x3F));
    } else {
      bytes = ((bytes & 0x0F) << 12) | ((this->read_byte() & 0x3F) << 6);
      bytes |= (this->read_byte() & 0x3F);
    }
    buffer[i] = bytes;
  }
}

/* Constructor. */
BufferedFileInput::BufferedFileInput(int32_t size) :
    buffer(NULL),
    size(size >= 0 ? size : BufferedFileOutput::BUFFER_SIZE),
    head(0),
    tail_offset(0),
    offset(0) {}

/* Copy constructor. */
BufferedFileInput::BufferedFileInput(const BufferedFileInput& clone):
    FileInput(clone),
    buffer(NULL),
    size(clone.size),
    head(clone.head),
    tail_offset(clone.tail_offset),
    offset(clone.offset) {
  if (clone.tail_offset != 0 && clone.buffer != NULL) {
    this->buffer = JARGON_NEW_ARRAY(uint8_t, this->tail_offset);
    memcpy(this->buffer, clone.buffer, this->tail_offset * sizeof(uint8_t));
  }
}

/* Returns object name. */
const char* BufferedFileInput::get_name() {
  return get_class_name();
}

/* Returns class name. */
const char* BufferedFileInput::get_class_name() {
  return "BufferedFileInput";
}

/* Reads a specified number of bytes into an array at the specified offset. */
void BufferedFileInput::read_bytes(uint8_t* buffer, const int32_t length){
  this->read_bytes(buffer, length, true);
}

/* Reads a specified number of bytes into an array at the specified offset. */
void BufferedFileInput::read_bytes(uint8_t* buffer, const int32_t length, bool use_buffer){
  int32_t request_length = length;
  uint8_t* request_buffer = buffer;
  if (request_length <= (this->tail_offset - this->offset)) {
    if (request_length > 0) {
      memcpy(request_buffer, this->buffer + this->offset, request_length);
    }
    this->offset += request_length;
  } else {
    int32_t available = this->tail_offset - this->offset;
    if (available > 0) {
      memcpy(request_buffer, this->buffer + this->offset, available);
      request_buffer += available;
      request_length -= available;
      this->offset += available;
    }
    if (use_buffer && request_length < this->size) {
      this->refill();
      if (this->tail_offset < request_length) {
	memcpy(request_buffer, this->buffer, this->tail_offset);
	JARGON_THROW(JARGON_ERR_IO, TLITERAL("exceed current buffer size."));
      } else {
	memcpy(request_buffer, this->buffer, request_length);
	this->offset = request_length;
      }
    } else {
      int64_t new_head = this->head + this->offset + request_length;
      if (new_head > this->length()) {
	JARGON_THROW(JARGON_ERR_IO, TLITERAL("exceed valid request size."));
      }
      read_internal(request_buffer, request_length);
      this->head = new_head;
      this->offset = 0;
      this->tail_offset = 0;
    }
  }
}

/* Returns the current position in this file. */
int64_t BufferedFileInput::get_pointer() const {
  return this->head + this->offset;
}

/* Sets current position in this file. */
void BufferedFileInput::seek(const int64_t position) {
  if (position < 0) {
    JARGON_THROW(JARGON_ERR_IO, TLITERAL("value must be a positive value."));
  }
  if (position >= this->head && position < (this->head + this->tail_offset)) {
    this->offset = (int32_t)(position - this->head);
  } else {
    this->head = position;
    this->offset = 0;
    this->tail_offset = 0;
    seek_internal(position);
  }
}

/* Closes this stream to further operations. */
void BufferedFileInput::close() {
  JARGON_DEL_ARRAY(this->buffer);
  this->tail_offset = 0;
  this->offset = 0;
  this->head = 0;
}

/* Deconstructor. */
BufferedFileInput::~BufferedFileInput() {
  BufferedFileInput::close();
}

/* Refills internal buffer. */
void BufferedFileInput::refill() {
  int64_t start = this->head + this->offset;
  int64_t end = start + this->size;
  if (end > this->length()) {
    end = this->length();
  }
  this->tail_offset = (int32_t)(end - start);
  if (this->tail_offset <= 0) {
    JARGON_THROW(JARGON_ERR_IO, TLITERAL("FileInput read past EOF"));
  }
  if (this->buffer == NULL){
    this->buffer = JARGON_NEW_ARRAY(uint8_t, this->size);
  }
  read_internal(this->buffer, this->tail_offset);
  this->head = start;
  this->offset = 0;
}

/* Sets buffer size. */
void BufferedFileInput::set_size(int32_t size) {
  if (size != this->size) {
    this->size = size;
    if (this->buffer != NULL) {
      uint8_t* new_buffer = JARGON_NEW_ARRAY(uint8_t, size);
      int32_t left_in_buffer = this->tail_offset - this->offset;
      int32_t number_to_copy;

      if (left_in_buffer > size) {
	number_to_copy = size;
      } else {
	number_to_copy = left_in_buffer;
      }

      memcpy( (void*)new_buffer, (void*)(this->buffer + this->offset), number_to_copy);
      this->head += this->offset;
      this->offset = 0;
      this->tail_offset = number_to_copy;

      JARGON_DEL_ARRAY(this->buffer);
      this->buffer = new_buffer;
    }
  }
}

JARGON_NAMESPACE_END
