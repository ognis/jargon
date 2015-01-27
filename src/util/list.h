/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#ifndef jargon_util_list_h
#define jargon_util_list_h

#include <list>
#include <vector>
#include <set>
#include "collection.h"
#include "object.h"

JARGON_NAMESPACE_BEGIN(util)

/* Class encaplulates various list implementations. */
template<
    typename ValueType,
    typename Base,
    typename ValueDeleter>
class AbstractList : public Base {
 public:
  typedef typename Base::const_iterator const_iterator;
  typedef typename Base::iterator iterator;

  /* Constructor. */
  AbstractList(const bool value_deletable) :
      value_deletable(value_deletable) {}

  /* Deconstructor. */
  virtual ~AbstractList() {
    this->clear();
  }

  /* Sets this value-deletability true. */
  void set_value_deletable(const bool value_deletable) {
    this->value_deletable = value_deletable;
  }

  /* Copies current list into assigned array. */
  void to_array(ValueType* array, const bool null_terminated=false) const {
    int i = 0;
    for (const_iterator iter = Base::begin(); iter != Base::end(); iter++ ) {
      array[i] = *iter;
      i++;
    }
    if (null_terminated) {
      array[i] = NULL;
    }
  }

  /* Sets value within assigned index. */
  void set(size_t index, ValueType value) {
    if (value_deletable && index < Base::size()) {
      ValueDeleter::do_deletion_of((*this)[index]);
    }
    if (index + 1 > Base::size()) {
      Base::resize(index + 1);
    }
    (*this)[index] = value;
  }

  /* Removes value of assigned index. */
  void remove(size_t index, const bool dont_delete_value=false){
    if (index < Base::size()) {
      iterator iter = Base::begin();
      iter += index;
      ValueType value = *iter;
      Base::erase(value);
      if (value_deletable && !dont_delete_value) {
	ValueDeleter::do_deletion_of(value);
      }
    }
  }

  /* Removes value of assigned iterator. */
  void remove(iterator iter, bool dont_delete_value=false) {
    ValueType value = *iter;
    Base::erase(iter);
    if (value_deletable && !dont_delete_value) {
      ValueDeleter::do_deletion_of(value);
    }
  }

  /* Removes value of the tail. */
  void delete_tail() {
    if (Base::size() > 0) {
      iterator iter = Base::end();
      if (iter != Base::begin()) {
	iter--;
      }
      ValueType value = *iter;
      Base::erase(iter);
      if (this->value_deletable) {
	ValueDeleter::do_deletion_of(value);
      }
    }
  }

  /* Removes value of the head. */
  void delete_head() {
    if (Base::size() > 0) {
      iterator iter = Base::begin();
      ValueType value = *iter;
      Base::erase(iter);
      if (this->value_deletable) {
	ValueDeleter::do_deletion_of(value);
      }
    }
  }

  /* Clears list. */
  void clear() {
    if (value_deletable) {
      iterator iter = Base::begin();
      while (iter != Base::end()) {
	ValueDeleter::do_deletion_of(*iter);
	++iter;
      }
    }
    Base::clear();
  }

 protected:
  bool value_deletable;
};

/* Class of growable arrays of Objects. */
template<
    typename ValueType,
    typename ValueDeleter=JARGON_NAMESPACE(util)::Freeable::Dummy>
class Vector : public AbstractList<
    ValueType,
    std::vector<ValueType>,
    ValueDeleter> {
 public:
  Vector(const bool value_deletable=true) :
    AbstractList<
        ValueType,
        std::vector<ValueType>,
        ValueDeleter>(value_deletable) {}
};

#define ArrayList Vector
#define HashSet HashList
#define List Vector

#if defined(JARGON_DISABLE_HASHING)
#  define HashList SetList
#else

/* Class of growable hash list of Objects. */
template<
    typename ValueType,
    typename Comparator=JARGON_NAMESPACE(util)::Ordered::TChar,
    typename ValueDeleter=JARGON_NAMESPACE(util)::Freeable::Dummy>
class HashList : public AbstractList<
    ValueType,
    std::JARGON_HASH_SET<ValueType, Comparator>,
    ValueDeletor> {
 public:
  HashList (const bool value_deletable=true) :
    AbstractList<
        ValueType,
        std::JARGON_HASH_SET)<ValueType, Comparator>,
        ValueDeletor>(value_deletable) {}
};
#endif

/* Class of growable linked list of Objects. */
template<
    typename ValueType,
    typename ValueDeleter=JARGON_NAMESPACE(util)::Freeable::Dummy>
class LinkedList : public AbstractList<
    ValueType,
    std::list<ValueType>,
    ValueDeleter> {
 public:
  LinkedList (const bool value_deletable=true) :
    AbstractList<
        ValueType,
        std::list<ValueType>,
        ValueDeleter>(value_deletable) {}
};

template<
    typename ValueType,
    typename Comparator=JARGON_NAMESPACE(util)::Ordered::TChar,
    typename ValueDeleter=JARGON_NAMESPACE(util)::Freeable::Dummy>
class SetList : public AbstractList<
    ValueType,
    std::set<ValueType, Comparator>,
    ValueDeleter> {
 public:
  SetList (const bool value_deletable=true) :
    AbstractList<
        ValueType,
        std::set<ValueType, Comparator>,
        ValueDeleter>(value_deletable) {}
};

JARGON_NAMESPACE_END

#endif /* jargon_util_list_h */
