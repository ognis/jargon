/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#ifndef jargon_util_map_h
#define jargon_util_map_h

#include "collection.h"
#include "object.h"

JARGON_NAMESPACE_BEGIN(util)

/* Class encaplulates various map implementations. */
template<
    typename KeyType,
    typename ValueType,
    typename Base,
    typename KeyDeleter=JARGON_NAMESPACE(util)::Freeable::Dummy,
    typename ValueDeleter=JARGON_NAMESPACE(util)::Freeable::Dummy>
class AbstractHashMap : public Base {
 public:
  typedef typename Base::iterator iterator;
  typedef typename Base::const_iterator const_iterator;
  typedef std::pair<KeyType, ValueType> pair;

  /* Constructor. */
  AbstractHashMap() : 
    key_deletable(true),
    value_deletable(true) {}

  /* Deconstructor. */
  ~AbstractHashMap() {
    this->clear();
  }

  /* Sets this key-deletability true. */
  void set_key_deletable(bool key_deletable) {
    this->key_deletable = key_deletable;
  }

  /* Sets this value-deletability true. */
  void set_value_deletable(bool value_deletable) {
    this->value_deletable = value_deletable;
  }

  /* Retruns true if the value which is associated with the key exists. */
  bool exists(KeyType key) const {
    const_iterator iter = Base::find(key);
    bool result = iter != Base::end();
    return result;
  }

  /* Retruns the value which is associated with the key. */
  ValueType get(KeyType key) {
    const_iterator iter = Base::find(key);
    if (iter == Base::end()) {
      return (ValueType) NULL;
    } else {
      return iter->second;
    }
  }

  /* Removes the value which is pointed by the iterator given. */
  void remove(
      iterator iter,
      const bool dont_delete_key=false,
      const bool dont_delete_value=false) {
    if (iter == Base::end()) {
      return;
    }

    KeyType key = iter->first;
    ValueType value = iter->second;
    Base::erase(iter);

    if (this->key_deletable && !dont_delete_key) {
      KeyDeleter::do_deletion_of(key);
    }

    if (this->value_deletable && !dont_delete_value) {
      ValueDeleter::do_deletion_of(value);
    }
  }

  /* Removes the value which is associated with the key. */
  void remove(
      KeyType key,
      const bool dont_delete_key=false,
      const bool dont_delete_value=false) {
    iterator iter = Base::find(key);
    if (iter != Base::end()) {
      remove(iter, dont_delete_key, dont_delete_value);
    }
  }

  /* Clears currently holding key-value pairs. */
  void clear() {
    if (this->key_deletable || this->value_deletable) {
      iterator iter = Base::begin();
      while (iter != Base::end()) {
#ifdef JARGON_HAVE_EXT_HASH_MAP
	remove(iter);
	iter = Base::begin();
#else
	if (this->key_deletable) {
	  KeyDeleter::do_deletion_of(iter->first);
	}

	if (this->value_deletable) {
	  ValueDeleter::do_deletion_of(iter->second);
	}

	++iter;
#endif
      }
    }
    Base::clear();
  }

 protected:
  bool key_deletable;
  bool value_deletable;
};

#if defined(JARGON_DISABLE_HASHING)

/* HashMap implementation template which is based on std::map implementation. */
template<
    typename KeyType,
    typename ValueType,
    typename Comparator,
    typename EquatorDummy,
    typename KeyDeleter=JARGON_NAMESPACE(util)::Freeable::Dummy,
    typename ValueDeleter=JARGON_NAMESPACE(util)::Freeable::Dummy>
class HashMap : public AbstractHashMap<
    KeyType,
    ValueType,
    std::map<KeyType, ValueType, Comparator>,
    KeyDeleter,
    ValueDeleter> {
  typedef AbstractHashMap<
      KeyType,
      ValueType,
      std::map<KeyType, ValueType, Comparator>,
      KeyDeleter,
      ValueDeleter
  > This;
 public:
  /* Constructor. */
  HashMap(const bool key_deletable=false, const bool value_deletable=false) {
    This::set_key_deletable(key_deletable);
    This::set_value_deletable(value_deletable);
  }

  /* Puts key-value pair which is assigned in the inner map implementation. */
  virtual void put(KeyType key, ValueType value) {
    if (This::key_deletable || This::value_deletable) {
      This::remove(key);
    }
    (*this)[key] = value;
  }
};

#elif defined(JARGON_HAVE_EXT_HASH_MAP_H)

/* HashMap implementation template which is based on ext/hash_map implementation. */
template<
    typename KeyType,
    typename ValueType,
    typename Hasher,
    typename Equator,
    typename KeyDeleter=JARGON_NAMESPACE(util)::Freeable::Dummy,
    typename ValueDeleter=JARGON_NAMESPACE(util)::Freeable::Dummy>
class HashMap : public AbstractHashMap<
    KeyType,
    ValueType,
    std::JARGON_HASH_MAP<KeyType, ValueType, Hasher, Equator>,
    KeyDeletor,
    ValueDeletor> {
  typedef AbstractHashMap<
      KeyType,
      ValueType,
      std::JARGON_HASH_MAP<KeyType, ValueType, Hasher, Comparator>,
      KeyDeleter,
      ValueDeleter
  > This;
 public:
  /* Constructor. */
  HashMap(const bool key_deletable=false, const bool value_deletable=false) {
    This::set_key_deletable(key_deletable);
    This::set_value_deletable(value_deletable);
  }

  /* Puts key-value pair which is assigned in the inner map implementation. */
  virtual void put(KeyType key, ValueType value) {
    if (This::key_deletable || This::value_deletable) {
      This::remove(key);
    }
    (*this)[key] = value;
  }
};

#elif defined(JARGON_HAVE_HASH_MAP)

/* HashMap implementation template which is based on hash_map implementation. */
template<
    typename KeyType,
    typename ValueType,
    typename Hasher,
    typename Equator,
    typename KeyDeleter=JARGON_NAMESPACE(util)::Freeable::Dummy,
    typename ValueDeleter=JARGON_NAMESPACE(util)::Freeable::Dummy>
class HashMap : public AbstractHashMap<
    KeyType,
    ValueType,
    std::JARGON_HASH_MAP<KeyType, ValueType, Hasher>,
    KeyDeletor,
    ValueDeletor> {
  typedef AbstractHashMap<
      KeyType,
      ValueType,
      std::JARGON_HASH_MAP<KeyType, ValueType, Hasher>,
      KeyDeletor,
      ValueDeletor
  > This;
 public:
  /* Constructor. */
  HashMap(const bool key_deletable=false, const bool value_deletable=false) {
    This::set_key_deletable(key_deletable);
    This::set_value_deletable(value_deletable);
  }

  /* Puts key-value pair which is assigned in the inner map implementation. */
  virtual void put(KeyType key, ValueType value) {
    if (This::key_deletable || This::value_deletable) {
      This::remove(key);
    }
    (*this)[key] = value;
  }
};
#endif

/* Set implementation template based on  std::map. */
template<
    typename ValueType,
    typename ParameterType,
    typename Comparator,
    typename ValueDeleter=JARGON_NAMESPACE(util)::Freeable::Dummy,
    typename ParameterDeleter=JARGON_NAMESPACE(util)::Freeable::Dummy>
class ParametrizedSet : public AbstractHashMap<
    ValueType,
    ParameterType,
    std::map<ValueType, ParameterType, Comparator>,
    ValueDeleter,
    ParameterDeleter> {
  typedef AbstractHashMap<
      ValueType,
      ParameterType,
      std::map<ValueType, ParameterType, Comparator>,
      ValueDeleter,
      ParameterDeleter
  > This;
 public:
  /* Constructor. */
  ParametrizedSet(const bool value_deletable=false, const bool parameter_deletable=false) {
    This::set_key_deletable(value_deletable);
    This::set_value_deletable(parameter_deletable);
  }

  /* Puts key-value pair which is assigned in the inner map implementation. */
  virtual void put(ValueType value, ParameterType parameter) {
    if (This::key_deletable || This::value_deletable) {
      This::remove(value);
    }
    (*this)[value] = parameter;
  }
};

/* MultiMap implementation template based on  std::multimap. */
template<
    typename KeyType,
    typename ValueType,
    typename Comparator,
    typename KeyDeleter=JARGON_NAMESPACE(util)::Freeable::Dummy,
    typename ValueDeleter=JARGON_NAMESPACE(util)::Freeable::Dummy>
class MultiMap : public AbstractHashMap<
    KeyType,
    ValueType,
    std::multimap<KeyType, ValueType>,
    KeyDeleter,
    ValueDeleter> {
  typedef AbstractHashMap<
      KeyType,
      ValueType,
      std::multimap<KeyType, ValueType>,
      KeyDeleter,
      ValueDeleter
  > This;
 public:
  /* Constructor. */
  MultiMap(const bool key_deletable=false, const bool value_deletable=false) {
    This::set_key_deletable(key_deletable);
    This::set_value_deletable(value_deletable);
  }

  /* Puts key-value pair which is assigned in the inner map implementation. */
  virtual void put(KeyType key, ValueType value) {
    if (This::key_deletable || This::value_deletable) {
      This::remove(key);
    }
    //    (*this)[key] = value;
  }
};

JARGON_NAMESPACE_END

#endif /* jargon_util_map_h */
