/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#ifndef jargon_io_fio_h
#define jargon_io_fio_h

#include "../shared/monolithic.h"
#include "../util/threading.h"
#include "../util/object.h"

JARGON_NAMESPACE_BEGIN(io)

/* Abstract base class of random-access input stream. */
class FileInput : public JARGON_NAMESPACE(util)::NamedObject {
 public:
  DEFINE_MUTEX(THIS_LOCK);

  /* Deconstructor. */
  virtual ~FileInput();

  /* Clones instance. */
  virtual FileInput* clone() const = 0;

  /* Reads byte from current position. */
  virtual uint8_t read_byte() = 0;

  /* Reads a specified number of bytes into an array at the specified offset. */
  virtual void read_bytes(uint8_t* buffer, const int32_t length) = 0;

  /* Reads a specified number of bytes into an array at the specified offset. */
  virtual void read_bytes(uint8_t* buffer, const int32_t length, bool use_buffer);

  /* Reads four bytes and returns an int. */
  int32_t read_int();

  /* Reads an int stored in variable-length format. */
  virtual int32_t read_variable_int();

  /* Reads eight bytes and returns a long. */
  int64_t read_long();

  /* Reads a long stored in variable-length format. */
  int64_t read_variable_long();

  /* Reads a string */
  int32_t read_string(tchar_t* buffer, const int32_t max);

#  ifndef JARGON_ASCII_MODE
  /* Reads a string and converts to ascii. */
  int32_t read_string(char* buffer, const int32_t max);
#  endif

  /* Reads a string. */
  tchar_t* read_string();

  /* Reads UTF-8 encoded characters into an array. */
  void read_chars(thcar_t* buffer, const int32_t start, const int32_t length);

  /* Skips assigned length characters. */
  void skip_chars(const int32_t length);

  /* Closes the stream to futher operations. */
  virtual void close() = 0;

  /* Returns the current position in this file. */
  virtual int64_t get_pointer() const = 0;

  /* Sets current position in this file, where the next read will occur. */
  virtual void seek(const int64_t position) = 0;

  /* The number of bytes in the file. */
  virtual int64_t length() const = 0;

  /* Returns assigned input type. */
  virtual const char* get_type() const = 0;

  /* Returns object name. */
  virtual const char* get_name() const = 0;

 protected:
  /* Constructor. */
  FileInput();

  /* Copy constructor. */
  FileInput(const FileInput& clone);
};

/* Abstract base class for buffered input from a file. */
class BufferedFileInput: public FileInput {
 public:
  JARGON_STATIC_CONSTANT(int32_t, BUFFER_SIZE=JARGON_STREAM_BUFFER_SIZE);

  /* Deconstructor. */
  virtual ~BufferedFileInput();

  /* Clones instance. */
  virtual FileInput* clone() const = 0;

  /* Closes the stream to futher operations. */
  void close();

  /* Reads byte from current position. */
  inline uint8_t read_byte() {
    if (this->offset >= this->tail_offset)
      this->refill();
    return this->buffer[this->offset++];
  }

  /* Reads a specified number of bytes into an array at the specified offset. */
  void read_bytes(uint8_t* buffer, const int32_t length);

  /* Reads a specified number of bytes into an array at the specified offset. */
  void read_bytes(uint8_t* buffer, const int32_t length, bool use_buffer);

  /* Returns the current position in this file. */
  int64_t get_pointer() const;

  /* Sets current position in this file, where the next read will occur. */
  void seek(const int64_t position);

  /* Sets buffer size. */
  void set_size(int32_t size);

  /* Returns object name. */
  const char* get_name();

  /* Returns class name. */
  static const char* get_class_name();

 protected:
  int32_t size;
  int64_t head;
  int32_t tail_offset;
  int32_t offset;

  /* Copy constructor. */
  BufferedFileInput(const BufferedFileInput& clone);

  /* Constructor. */
  BufferedFileInput(int32_t size = -1);

  /* Internal implementation of buffer refill. */
  virtual void read_internal(uint8_t* buffer, const int32_t length) = 0;

  /* Internal implementation of seek. */
  virtual void seek_internal(const int64_t position) = 0;

 private:
  uint8_t* buffer;
  void refill();
};

/* Abstract base class of random-access output stream. */
class FileOutput {
 public:
  /* Constructor. */
  FileOutput();

  /* Deconstructor. */
  virtual ~FileOutput();

  /* Writes a single byte. */
  virtual void write_byte(const uint8_t value) = 0;

  /* Writes an array of bytes. */
  virtual void write_bytes(const uint8_t* value, const int32_t length) = 0;

  /* Writes an int as four bytes. */
  void write_int(const int32_t value);

  /* Writes an int in a variable-length format. */
  void write_variable_int(const int32_t value);

  /* Writes a long as eight bytes. */
  void write_long(const int64_t value);

  /* Writes an long in a variable-length format. */
  void write_variable_long(const int64_t value);

  /* Writes a string. */
  void write_string(const tchar_t* value, const int32_t length);

  /* Writes a string. */
  void write_string(const std::string& value);
	
#  ifndef JARGON_ASCII_MODE
  /* Writes an ascii string. */
  void write_string(const char* value, const int32_t length);
#  endif

  /* Writes a sequence of UTF-8 encoded characters from a string. */
  void write_chars(const tchar_t* value, const int32_t length);

  /* Closes this stream to further operations. */
  virtual void close() = 0;

  /* Returns the current position in this file. */
  virtual int64_t get_pointer() const = 0;

  /* Sets current position in this file. */
  virtual void seek(const int64_t position) = 0;

  /* The number of bytes in the file. */
  virtual int64_t length() const = 0;

  /* Forces any buffered output to be written. */
  virtual void flush() = 0;

  /* Copy bytes from input to ourself. */
  void copy_bytes(FileInput* input, int64_t bytes);

private:
  JARGON_STATIC_CONSTANT(int32_t, COPY_BUFFER_SIZE=16384);
  bool closed;
  uint8_t* copy_buffer;
};

/* Abstract base class for buffered output to a file. */
class BufferedFileOutput : public FileOutput {
 public:
  LUCENE_STATIC_CONSTANT(int32_t, BUFFER_SIZE=16384);

  /* Constructor. */
  BufferedFileOutput();

  /* Deconstructor. */
  virtual ~BufferedFileOutput();

  /* Writes a single byte. */
  virtual void write_byte(const uint8_t value);

  /* Writes an array of bytes. */
  virtual void write_bytes(const uint8_t* value, const int32_t length);

  /* Closes this stream to further operations. */
  virtual void close();

  /* Returns the current position in this file, where the next write will occur. */
  int64_t get_pointer() const;

  /* Sets current position in this file. */
  virtual void seek(const int64_t position);

  /* The number of bytes in the file. */
  virtual int64_t length() const = 0;

  /** Forces any buffered output to be written. */
  void flush();

protected:
  /* Writes bytes at the current position in the output. */
  virtual void flush_buffer(const uint8_t* value, const int32_t length) = 0;

 private:
  uint8_t* buffer;
  int64_t head;
  int32_t offset;
};

JARGON_NAMESPACE_END

#endif /* jargon_io_fio_h */
