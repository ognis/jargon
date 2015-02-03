/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#ifndef jargon_directory_lock_h
#define jargon_directory_lock_h

#include <climits>
#include <string.h>
#include "../shared/monolithic.h"
#include "../util/list.h"
#include "../util/object.h"
#include "../util/threading.h"

JARGON_NAMESPACE_BEGIN(directory)

/* Lock manager is responsible for managing instanciated locks. */
class LockManager : public JARGON_NAMESPACE(util)::HashSet<
    const char*,
    JARGON_NAMESPACE(util)::Ordered::Char> {
 public:
  /* Constructor. */
  LockManager() {
    this->set_value_deletable(false);
  }

  /* Deconstructor. */
  virtual ~LockManager() {}
};

/* Abstract class of lock. */
class AbstractLock : public JARGON_NAMESPACE(util)::NamedObject {
 public:
  JARGON_STATIC_CONSTANT(int64_t, LOCK_POLL_INTERVAL = 1000);
  JARGON_STATIC_CONSTANT(int64_t, LOCK_OBTAIN_WAIT_FOREVER = -1);

  /* Attempts to obtain exclusive access and immediately return
     upon success or failure. */
  virtual bool obtain() = 0;

  /* Attempts to obtain an exclusive lock within amount of time given. */
  bool obtain(int64_t timeout);

  /* Release exclusive access. */
  virtual void release() = 0;

  /* Returns true if the resource is currently locked. */
  virtual bool is_locked() = 0;

  /* Deconstructor. */
  virtual ~AbstractLock();

  /* Stringify lock instance. */
  virtual std::string to_string() = 0;
};

/* Class of single instanciated lock. */
class SingleInstanceLock: public AbstractLock {
 public:
  /* Constructor. */
  SingleInstanceLock(
      LockManager* manager,
      JARGON_THREAD_MUTEX* MANAGER_LOCK,
      const char* name);

  /* Deconstructor. */
  virtual ~SingleInstanceLock();

  /* Attempts to obtain exclusive access and immediately return
     upon success or failure. */
  bool obtain();

  /* Release exclusive access. */
  void release();

  /* Returns true if the resource is currently locked. */
  bool is_locked();

  /* Stringify lock instance. */
  std::string to_string();

  /* Returns class name. */
  static const char* get_class_name();

  /* Returns object name. */
  const char* get_name() const;

 private:
  const char* name;
  LockManager* manager;
  DEFINE_MUTEX(*MANAGER_LOCK);
};

/* Class of no locking. */
class NoLock: public AbstractLock {
 public:
  /* Attempts to obtain exclusive access and immediately return
     upon success or failure. */
  bool obtain();

  /* Release exclusive access. */
  void release();

  /* Returns true if the resource is currently locked. */
  bool is_locked();

  /* Stringify lock instance. */
  std::string to_string();

  /* Returns class name. */
  static const char* get_class_name();

  /* Returns object name. */
  const char* get_name() const;
};

/* Class of file system based lock. */
class FSLock: public AbstractLock {
 public:
  /* Constructor. */
  FSLock(const char* path, const char* name, int mode=-1);

  /* Deconstructor. */
  ~FSLock();

  /* Attempts to obtain exclusive access and immediately return
     upon success or failure. */
  bool obtain();

  /* Release exclusive access. */
  void release();

  /* Returns true if the resource is currently locked. */
  bool is_locked();

  /* Stringify lock instance. */
  std::string to_string();

  /* Returns class name. */
  static const char* get_class_name();

  /* Returns object name. */
  const char* get_name() const;

 private:
  char* file_name;
  char* directory_name;
  int mode;
};

/* Abstract class of lock factory. */
class AbstractLockFactory {
 public:
  /* Constructor. */
  AbstractLockFactory();

  /* Deconstructor. */
  virtual ~AbstractLockFactory();

  /* Returns lock instance prefix. */
  void set_prefix(const char* prefix);

  /* Sets lock instance prefix. */
  const char* get_prefix();

  /* Make lock instance. */
  virtual AbstractLock* make(const char* name) = 0;

  /* Clear lock instance. */
  virtual void clear(const char* name)=0;
 protected:
  std::string prefix;
};

/* Factory class of single instanciated lock. */
class SingleInstanceLockFactory: public AbstractLockFactory {
 public:
  /* Constructor. */
  SingleInstanceLockFactory();

  /* Deconstructor. */
  ~SingleInstanceLockFactory();

  /* Make lock instance. */
  AbstractLock* make(const char* name);

  /* Clear lock instance. */
  void clear(const char* name);
 private:
  LockManager* manager;
  DEFINE_MUTEX(MANAGER_LOCK);
};

/* Factory class of stab lock. */
class NoLockFactory: public AbstractLockFactory {
public:
  /* Returns singleton instance. */
  static NoLockFactory* get_factory();

  /* Make lock instance. */
  AbstractLock* make(const char* name);

  /* Clear lock instance. */
  void clear(const char* name);

  /* Called when platform_shutdown is called. */
  static void shutdown();

  static NoLockFactory* singleton_factory;
  static NoLock* singleton_lock;
};

/* Factory class of file system based lock. */
class FSLockFactory: public AbstractLockFactory {
 public:
  /* Constructor. */
  FSLockFactory(const char* path=NULL, int mode=-1);

  /* Deconstructor. */
  ~FSLockFactory();

  /* Sets directory path. */
  void set_path(const char* path);

  /* Make lock instance. */
  AbstractLock* make(const char* name);

  /* Clear lock instance. */
  void clear(const char* name);

  /* Returns class name. */
  static const char* get_class_name();

  /* Returns class name. */
  const char* get_name();
 private:
  std::string path;
  int mode;
};

JARGON_NAMESPACE_END

#endif /* jargon_directory_lock_h */
