/*
 * Copyright (C) 2014- Shingo OKAWA
 */
#ifndef jargon_util_object_h
#define jargon_util_object_h

#include <cstdlib>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "../shared/monolithic.h"

JARGON_NAMESPACE_BEGIN(util)

/* NamedObject represents object with specified name. */
class NamedObject {
 public:
  virtual ~NamedObject();
  virtual const char* get_name() const = 0;
  virtual bool instance_of(const char* name) const;
};

/* Comparable represents object which is comparable. */
class Comparable : public NamedObject {
 public:
  virtual ~Comparable() {}
  virtual int32_t compare_to(NamedObject* object) = 0;
};

/* Equators is responsible for evaluate equality. */
class Equator {
 public:
  class Char : public std::binary_function<const char*, const char*, bool> {
   public:
    bool operator()(const char* left, const char* right) const;
  };

  class Int32 : public std::binary_function<const int32_t*, const int32_t*, bool> {
   public:
    bool operator()(const int32_t left, const int32_t right) const;
  };

#ifndef JARGON_ASCII_MODE
  class WChar : public std::binary_function<const wchar_t*, const wchar_t*, bool> {
   public:
    bool operator()(const wchar_t* left, const wchar_t* right) const;
  };

  class TChar : public WChar {
  };
#else
  class TChar : public Char {
  };
#endif

  template<typename Class>
  class Void : public std::binary_function<const void*, const void*, bool> {
   public:
    bool operator()(Class* left, Class* right) const{
      return left == right;
    }
  };
};

/* Comparators is responsible for evaluate inequality. */
class Comparator {
 public:
  class Base {
   public:
    enum {
      bucket_size = 4,
      min_buckets = 8
    };
    Base() {
    }
  };

  class Int32 : public Base, public Comparable {
    int32_t value;
   public:
    Int32();
    Int32(int32_t value);
    int32_t get_value() const;
    int32_t compare_to(NamedObject* object);
    bool operator()(int32_t left, int32_t right) const;
    size_t operator()(int32_t value) const;
    static const char* get_class_name();
    const char* get_name() const;
  };

  class Float : public Comparable {
    float_t value;
   public:
    Float(float_t value);
    float_t get_value() const;
    int32_t compare_to(NamedObject* object);
    static const char* get_class_name();
    const char* get_name() const;
  };

  class Char : public Base, public Comparable {
    const char* value;
   public:
    Char();
    Char(const char* value);
    const char* get_value() const;
    int32_t compare_to(NamedObject* object);
    bool operator()( const char* left, const char* right) const;
    size_t operator()(const char* value) const;
    static const char* get_class_name();
    const char* get_name() const;
  };

#ifndef JARGON_ASCII_MODE
  class WChar : public Base, public Comparable {
    const wchar_t* value;
   public:
    WChar();
    WChar(const wchar_t* value);
    const wchar_t* get_value() const;
    int32_t compare_to(NamedObject* object);
    bool operator()(const wchar_t* left, const wchar_t* right) const;
    size_t operator()(const wchar_t* value) const;
    static const char* get_class_name();
    const char* get_name() const;
  };
  typedef WChar TChar;
#else
  typedef Char TChar;
#endif

  template<typename Class>
  class Void : public Base {
   public:
    int32_t compare_to(Class* object) {
      if (this == object) {
	return object;
      } else {
	return this > object ? 1 : -1;
      }
    }
    bool operator()(Class* left, Class* right) const {
      return left > right ? true : false;
    }
    size_t operator()(Class* value) const {
      return (size_t)value;
    }
  };
};

int32_t compare(Comparable* left, Comparable* right);

class AbstractDeletor {
 public:
  virtual void delete(void*) = 0;
  virtual ~AbstractDeletor();
};

/* Deletros is responsible for deleting object arrays. */
class Deletor {
 public:
  class TCharArray : public AbstractDeletor {
   public:
    void delete(void* array) {
      exec((tchar_t*)array);
    }
    static void exec(tchar_t* array) {
      JARGON_FREE(array);
    }
  };

  template<typename Type>
  class ValueArray : public AbstractDeletor {
   public:
    void delete(void* array) {
      exec((Type*)array);
    }
    static void exec(Type* array) {
      JARGON_FREE(array);
    }
  };

  template<typename Class>
  class ObjectArray : public AbstractDeletor {
   public:
    void delete(void* array) {
      exec((Class*)array);
    }
    static void exec(Class* array) {
      JARGON_DEL_ARRAY(array);
    }
  };

  class CharArray : public AbstractDeletor {
   public:
    void delete(void* array) {
      exec((char*)array);
    }
    static void exec(char* array) {
      JARGON_FREE(array);
    }
  };

  template<typename Class>
  class Object : public AbstractDeletor {
   public:
    void delete(void* object){
      exec((Class*)object);
    }
    static void exec(Class* obj){
      JARGON_FREE(obj);
    }
  };

  template<typename Class>
  class Void : public AbstractDeletor {
   public:
    void delete(void* object){
      exec((Class*)object);
    }
    static void exec(Class* object){
      JARGON_FREE(object);
    }
  };

  class Dummy: public AbstractDeletor {
   public:
    void delete(void*) {}
    static void exec(const void*) {}
  };

  class DummyInt32 : public AbstractDeletor {
   public:
    void delete(void*) {}
    static void exec(const int32_t) {}
  };

  class DummyFloat : public AbstractDeletor {
   public:
    void delete(void*) {}
    static void exec(const float_t) {}
  };

  template <typename Type>
  class ConstNullValue : public AbstractDeletor {
   public:
    void delete(void*) {}
    static void exec(const Type) {}
  };

  template <typename Type>
  class NullValue : public AbstractDeletor {
   public:
    void delete(void*) {}
    static void exec(Type) {}
  };
};

JARGON_NAMESPACE_END

#endif /* jargon_util_object_h */
