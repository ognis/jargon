/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#ifndef jargon_util_array_h
#define jargon_util_array_h

#include <cstdlib>
#include <string.h>
#include "../shared/monolithic.h"
#include "../debug/error.h"

JARGON_NAMESPACE_BEGIN(util)

template<typename ValueType>
class AbstractArray {
 public:
  ValueType* values;
  size_t length;

  /* Constructor. */
  AbstractArray(const size_t length=0) :
    values(NULL),
    length(length) {
    if (length > 0) {
      this->values = (ValueType*)malloc(sizeof(ValueType) * length);
      memset(this->values, 0, sizeof(ValueType) * length);
    }
  }

  /* Constructor. */
  AbstractArray(ValueType* values, const size_t length) :
    values(values),
    length(length) {}

  /* Delete single value. */
  virtual void delete_value(ValueType value) = 0;

  /* Delete values in array. */
  virtual void delete_all() {
    if (this->values == NULL) {
      return;
    }
    for (size_t i = 0; i < this->length; i++) {
      this->delete_value(this->values[i]);
    }
  }

  /* Delete values in array and free memory. */
  void clear() {
    this->delete_all();
    this->free_internal();
  }

  /* Delete and free array holding values. */
  void free_internal(){
    free(this->values);
    this->values = NULL;
  }

  /* Deconstructor. */
  virtual ~AbstractArray() {}

  /* Return holding array and empty it. */
  ValueType* get_array(){
    ValueType* result = this->values;
    this->values = NULL;
    return result;
  }

  /* Access value of assigned index. */
  const ValueType& operator[](const size_t index) const {
    if (this->length <= index) {
      JARGON_THROW(JARGON_ERR_ILLEGAL_ARGUMENT, TLITERAL("array index out of range"));
    }
    return (*(this->values + index));
  }

  /* Access value of assigned index. */
  ValueType& operator[](const size_t index) {
    if (this->length <= index) {
      JARGON_THROW(JARGON_ERR_ILLEGAL_ARGUMENT, TLITERAL("array out of range"));
    }
    return (*(this->values + index));
  }

  /* Resize  array. */
  void resize(const size_t new_size, const bool delete_values=false) {
    if (this->length == new_size) {
      return;
    }

    if (this->values == NULL ) {
      this->values = (ValueType*)malloc(sizeof(ValueType) * new_size);
      memset(this->values, 0, sizeof(ValueType) * new_size);
      this->length = new_size;
      return;
    }

    if (this->length < new_size) {
      this->values = (ValueType*)realloc(this->values, sizeof(ValueType) * new_size);
      memset(this->values + this->length, 0, sizeof(ValueType) * (new_size - this->length));
    } else {
      if (delete_values) {
	for (size_t i = new_size; i < this->length; i++) {
	  delete_value(this->values[i]);
	}
      }

      if (new_size == 0) {
	free(this->values);
	this->values = NULL;
      } else {
	this->values = (ValueType*)realloc(this->values, sizeof(ValueType) * new_size);
      }
    }
    this->length = new_size;
  }
};

/* Class of array of objects. */
template<typename ValueType>
class ObjectArray : public AbstractArray<ValueType*> {
 public:
  /* Constructor. */
  ObjectArray() : 
    AbstractArray<ValueType*>() {}

  /* Constructor. */
  ObjectArray(ValueType** values, size_t length) :
    AbstractArray<ValueType*>(values, length) {}

  /* Constructor. */
  ObjectArray(size_t length) :
    AbstractArray<ValueType*>(length) {}

  /* Delete single value. */
  void delete_value(ValueType* value) {
    JARGON_FREE(value);
  }

  /* Delete values in array. */
  void delete_all() {
    if (this->values == NULL) {
      return;
    }
    for (size_t i = 0; i < this->length; ++i) {
      JARGON_FREE(this->values[i]);
    }
    this->free_internal();
  }

  /* Deconstructor. */
  virtual ~ObjectArray() {
    this->delete_all();
  }

  /* Initializes all cells in the array with a NULL value */
  void init() {
    for (size_t i = 0; i < this->length; i++) {
      this->values[i]=NULL;
    }
  }

  /* Delete arrays untill NULL appears and free memory. */
  void delete_until_null() {
    if (this->values == NULL) {
      return;
    }
    for (size_t i = 0; i < this->length && this->values[i] != NULL; ++i) {
      JARGON_FREE(this->values[i]);
    }
    this->free_internal();
  }
};

/* Class of array of which values do not need to be freed. */
template<typename ValueType>
class ValueArray : public AbstractArray<ValueType> {
 public:
  /* Constructor. */
  ValueArray() :
    AbstractArray<ValueType>() {}

  /* Constructor. */
  ValueArray(ValueType* values, size_t length) :
    AbstractArray<ValueType>(values, length) {}

  /* Constructor. */
  ValueArray(size_t length) :
    AbstractArray<ValueType>(length) {}

  /* Delete single value. */
  void delete_value(ValueType /* value */) {}

  /* Delete values in array. */
  void delete_all() {
    if (this->values == NULL) {
      return;
    }
    this->free_internal();
  }

  /* Deconstructor. */
  virtual ~ValueArray() {
    this->delete_all();
  }
};

/* Class of array of which values do not need to be freed and constant. */
template<typename ValueType>
class ConstValueArray : public AbstractArray<ValueType> {
 public:
  /* Constractor. */
  ConstValueArray() :
    AbstractArray<ValueType>() {}

  /* Constractor. */
  ConstValueArray(ValueType* values, size_t length) :
    AbstractArray<ValueType>(values, length) {}

  /* Constractor. */
  ConstValueArray(size_t length) :
    AbstractArray<ValueType>(length) {}

  /* Delete single value. */
  void delete_value(ValueType /* value */ ) {}

  /* Delete values in array. */
  void delete_all() {}

  /* Deconstructor. */
  virtual ~ConstValueArray() {}
};

/* Class of array of which values are tchar_t strings. */
class TCharArray : public AbstractArray<tchar_t*> {
 public:
  /* Constructor. */
  TCharArray() :
    AbstractArray<tchar_t*>() {}

  /* Constructor. */
  TCharArray(size_t length) :
    AbstractArray<tchar_t*>(length) {}

  /* Delete single value. */
  void delete_value(tchar_t* value) {
    JARGON_FREE(value);
  }

  /* Delete values in array. */
  void delete_all() {
    if (this->values == NULL) {
      return;
    }
    for (size_t i = 0; i < this->length; i++) {
      JARGON_FREE(this->values[i]);
    }
    this->free_internal();
  }

  /* Deconstructor. */
  virtual ~TCharArray(){
    this->delete_all();
  }
};

/* Class of array of which values are char strings. */
class CharArray : public AbstractArray<char*> {
 public:
  /* Constructor. */
  CharArray() :
    AbstractArray<char*>() {}

  /* Constructor. */
  CharArray(size_t length) :
    AbstractArray<char*>(length) {}

  /* Delete values in array. */
  void delete_all() {
    if (this->values == NULL) {
      return;
    }
    for (size_t i = 0; i < this->length; i++) {
      JARGON_FREE(this->values[i]);
    }
    this->free_internal();
  }

  /* Deconstructor. */
  virtual ~CharArray() {
    this->delete_all();
  }
};

/* Class of array of which tchar_t values do not need to be freed and constant. */
class ConstTCharArray : public AbstractArray<const tchar_t*> {
 public:
  /* Constructor. */
  ConstTCharArray() :
    AbstractArray<const tchar_t*>() {}

  /* Constructor. */
  ConstTCharArray(size_t length) :
    AbstractArray<const tchar_t*>(length) {}

  /* Delete values in array. */
  void delete_all() {
    if (this->values == NULL) {
      return;
    }
    this->free_internal();
  }

  /* Deconstructor. */
  virtual ~ConstTCharArray() {
    this->delete_all();
  }
};

/* Class of array of which char values do not need to be freed and constant. */
class ConstCharArray : public AbstractArray<const char*> {
 public:
  /* Constructor. */
  ConstCharArray() :
    AbstractArray<const char*>() {}

  /* Constructor. */
  ConstCharArray(size_t length) :
    AbstractArray<const char*>(length) {}

  /* Delete values in array. */
  void delete_all() {
    if (this->values == NULL) {
      return;
    }
    this->free_internal();
  }

  /* Deconstructor. */
  virtual ~ConstCharArray() {
    this->delete_all();
  }
};

template<typename ValueType>
class Arrays {
  /* Fill up array with assigned value. */
  static void fill(ValueType* array, int32_t length, ValueType value) {
    for (int32_t i = 0; i < length; i++) {
      array[i] = value;
    }
  }
};

JARGON_NAMESPACE_END

#endif
