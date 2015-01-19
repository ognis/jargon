/*
 * Copyright (C) 2014- Shingo OKAWA
 */
#include "object.h"
#include "misc.h"

JARGON_NAMESPACE_BEGIN(util)

/* Deconstructor. */
NamedObject::~NamedObject() {}

/* Returns true if the object is instance of the assigned class. */
bool NamedObject::instance_of(const char* name) const {
  const char* this_name = this->get_name();
  if (this_name == name || strcmp(this_name, name)==0) {
    return true;
  } else {
    return false;
  }
}

/* Returns true if two values are supposed to be equal. */
bool Equator::Int32::operator()(const int32_t left, const int32_t right) const {
  return left == right;
}

/* Returns true if two values are supposed to be equal. */
bool Equator::Char::operator()(const char* left, const char* right) const {
  if (left == right) {
    return true;
  }
  return (strcmp(left, right) == 0);
}

#ifndef JARGON_ASCII_MODE
/* Returns true if two values are supposed to be equal. */
bool Equator::WChar::operator()(const wchar_t* left, const wchar_t* right) const {
  if (left == right) {
    return true;
  }
  return (tcscmp(left, right) == 0);
}
#endif

/* Deconstructor. */
AbstractDeletor::~AbstractDeletor() {}

/* Compares two objects. */
int32_t compare(Comparable* left, Comparable* right) {
  if (left == NULL && right == NULL) {
    return 0;
  } else if (left == NULL) {
    return 1;
  } else if (right == NULL) {
    return -1;
  } else {
    return left->compare_to(right);
  }
}

/* Constructor. */
Comparator::Int32::Int32() {
  this->value = 0;
}

/* Constructor. */
Comparator::Int32::Int32(int32_t value) {
  this->value = value;
}

/* Returns assigned value. */
int32_t Comparator::Int32::get_value() const {
  return this->value;
}

/* Returns class name. */
const char* Comparator::Int32::get_class_name() {
  return "Comparator::Int32::get_class_name";
}

/* Returns name. */
const char* Comparator::Int32::get_name() const {
  return get_class_name();
}

/* Compares to object. */
int32_t Comparator::Int32::compare_to(NamedObject* object) {
  if (object->get_name() != Int32::get_class_name()) {
    return -1;
  }

  Int32* other = (Int32*)object;
  if (this->value == other->value) {
    return 0;
  }

  return this->value > other->value ? 1 : -1;
}

/* Compares two objects. */
bool Comparator::Int32::operator()(int32_t left, int32_t right) const {
  return left > right ? true : false;
}

size_t Comparator::Int32::operator()(int32_t value) const {
	return value;
}

/* Constructor. */
Comparator::Float::Float(float_t value) {
  this->value = value;
}

/* Returns assigned value. */
float_t Comparator::Float::get_value() const {
  return this->value;
}

/* Returns class name. */
const char* Comparator::Float::get_class_name() {
  return "Comparator::Float::get_class_name";
}

/* Returns class name. */
const char* Comparator::Float::get_name() const {
  return get_class_name();
}

/* Compares to object. */
int32_t Comparator::Float::compare_to(NamedObject* object) {
  if (object->get_name() != Float::get_class_name()) {
    return -1;
  }

  Float* other = (Float*)object;
  if (this->value == other->value) {
    return 0;
  }

  return this->value > other->value ? 1 : -1;
}

/* Construcotr. */
Comparator::Char::Char() {
  this->value = NULL;
}

/* Construcotr. */
Comparator::Char::Char(const char* value) {
  this->value = value;
}

/* Returns assigned value. */
const char* Comparator::Char::get_value() const {
  return this->value;
}

/* Returns class name. */
const char* Comparator::Char::get_class_name() {
  return "Comparator::Char::get_class_name";
}

/* Returns class name. */
const char* Comparator::Char::get_name() const{
  return get_class_name();
}

/* Compares to object. */
int32_t Comparator::Char::compare_to(NamedObject* object) {
  if (object->get_name() != Char::get_class_name()) {
    return -1;
  }
  Char* other = (Char*)object;
  return strcmp(this->value, other->value);
}

/* Compares two objects. */
bool Comparator::Char::operator()(const char* left, const char* right) const {
  if (left == right) {
    return false;
  }
  return (strcmp(left, right) < 0);
}

size_t Comparator::Char::operator()(const char* value) const {
  return JARGON_NAMESPACE(util)::misc::a_hash_code(value);
}

#ifndef JARGON_ASCII_MODE
/* Constructor. */
Comparator::WChar::WChar() {
  this->value = NULL;
}

/* Constructor. */
Comparator::WChar::WChar(const wchar_t* value) {
  this->value = value;
}

/* Returns assigned value. */
const wchar_t* Comparator::WChar::get_value() const {
  return this->value;
}

/* Returns class name. */
const char* Comparator::WChar::get_class_name() {
  return "Comparator::WChar::get_class_name";
}

/* Returns name. */
const char* Comparator::WChar::get_name() const {
  return get_class_name();
}

/* Compares to object. */
int32_t Comparator::WChar::compare_to(NamedObject* object) {
  if (object->get_name() != WChar::get_class_name()) {
    return -1;
  }
  tchar_t* other = (TChar*)object;
  return tcscmp(this->value, other->value);
}

/* Compares two objects. */
bool Comparator::WChar::operator()(const wchar_t* left, const wchar_t* right) const {
  if (left == right) {
    return false;
  }
  bool result = (tcscmp(left, right) < 0);
  return result;
}

size_t Comparator::WChar::operator()( const wchar_t* value) const {
  return JARGON_NAMESPACE(util)::misc::w_hash_code(value);
}
#endif /* JARGON_ASCII_MODE */

JARGON_NAMESPACE_END
