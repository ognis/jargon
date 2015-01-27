/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#include "lock.h"
#include "../debug/error.h"
#include "../util/misc.h"

#ifdef JARGON_HAVE_IO_H
#  ifdef __APPLE__
#    define POSSIBLE_O_RANDOM 0
#    include <sys/uio.h>
#  else
#    define POSSIBLE_O_RANDOM _O_RANDOM
#    include <io.h>
#  endif
#endif
#ifdef JARGON_HAVE_SYS_STAT_H
#  include <sys/stat.h>
#endif
#ifdef JARGON_HAVE_UNISTD_H
#  include <unistd.h>
#endif
#ifdef JARGON_HAVE_DIRECT_H
#  include <direct.h>
#endif
#include <fcntl.h>

JARGON_NAMESPACE_BEGIN(directory)

/* Deconstructor. */
AbstractLock::~AbstractLock() {}

/* Attempts to obtain exclusive access and immediately return
   upon success or failure. */
bool AbstractLock::obtain(int64_t timeout) {
  bool locked = this->obtain();
  if (timeout < 0 && timeout != LOCK_OBTAIN_WAIT_FOREVER) {
    JARGON_THROW(JARGON_ERR_ILLEGAL_ARGUMENT,
		 TLITERAL("timeout should be LOCK_OBTAIN_WAIT_FOREVER or a non-negative number"));
  }

  int64_t max_sleep_count = timeout / LOCK_POLL_INTERVAL;
  int64_t sleep_count = 0;

  while (!locked) {
    if (timeout != LOCK_OBTAIN_WAIT_FOREVER && sleep_count++ == max_sleep_count) {
      JARGON_THROW(JARGON_ERR_IO, TLITERAL("lock obtain timed out"));
    }
    JARGON_NAMESPACE(util)::misc::sleep(LOCK_POLL_INTERVAL);
    locked = this->obtain();
  }
  return locked;
}

/* Constructor. */
SingleInstanceLock::SingleInstanceLock(
    LockManager* manager,
    JARGON_THREAD_MUTEX* MANAGER_LOCK,
    const char* name) {
  this->manager = manager;
#ifndef JARGON_DISABLE_MULTITHREADING
  this->MANAGER_LOCK = MANAGER_LOCK;
#endif
  this->name = name;
}

/* Deconstructor. */
SingleInstanceLock::~SingleInstanceLock() {
}

/* Attempts to obtain exclusive access and immediately return
   upon success or failure. */
bool SingleInstanceLock::obtain() {
  SCOPED_LOCK_MUTEX(*MANAGER_LOCK);
  return manager->insert(this->name).second;
}

/* Release exclusive access. */
void SingleInstanceLock::release() {
  SCOPED_LOCK_MUTEX(*MANAGER_LOCK);
  LockManager::iterator iter = this->manager->find(this->name);
  if (iter != this->manager->end()) {
    this->manager->remove(iter, true);
  }
}

/* Returns true if the resource is currently locked. */
bool SingleInstanceLock::is_locked() {
  SCOPED_LOCK_MUTEX(*MANAGER_LOCK);
  return this->manager->find(this->name) == this->manager->end();
}

/* Stringify lock instance. */
std::string SingleInstanceLock::to_string() {
  return std::string("SingleInstanceLock:") + this->name;
}

/* Returns class name. */
const char* SingleInstanceLock::get_class_name() {
  return "SingleInstanceLock";
}

/* Returns object name. */
const char* SingleInstanceLock::get_name() const {
  return get_class_name();
}

/* Attempts to obtain exclusive access and immediately return
   upon success or failure. */
bool NoLock::obtain() {
  return true;
}

/* Release exclusive access. */
void NoLock::release() {}

  /* Returns true if the resource is currently locked. */
bool NoLock::is_locked() {
  return false;
}

/* Stringify lock instance. */
std::string NoLock::to_string() {
  return "NoLock";
}

/* Returns class name. */
const char* NoLock::get_class_name() {
  return "NoLock";
}

/* Returns object name. */
const char* NoLock::get_name() const {
  return get_class_name();
}

/* Constructor. */
FSLock::FSLock(const char* path, const char* name, int mode) {
  if (mode <= 0) {
    this->mode = 0644;
  } else {
    this->mode = mode;
  }
  this->file_name = JARGON_NEW_ARRAY(char, JARGON_MAX_PATH);
  this->directory_name = STRDUP_AtoA(path);
  strcpy(this->file_name, path);
  strcat(this->file_name, JARGON_PATH_DELIMITER_A);
  strcat(this->file_name, name);
}

/* Deconstructor. */
FSLock::~FSLock() {
  JARGON_DEL_ARRAY(this->file_name);
  JARGON_DEL_ARRAY(this->directory_name);
}

/* Attempts to obtain exclusive access and immediately return
   upon success or failure. */
bool FSLock::obtain() {
  if (!JARGON_NAMESPACE(util)::misc::directory_exists(this->directory_name)) {
    if (tmkdir(this->directory_name) == -1) {
      char* what = JARGON_NEW_ARRAY(char, 34 + strlen(this->directory_name) + 1);
      strcpy(what, "could not create lock directory: ");
      strcat(what, this->directory_name);
      JARGON_THROW(JARGON_ERR_IO, STRDUP_AtoT(what));
    }
  }
  int32_t result = topen(this->file_name,
		    O_RDWR | O_CREAT | POSSIBLE_O_RANDOM | O_EXCL,
		    this->mode); 
  if (result < 0) {
    return false;
  } else {
    tclose(result);
    return true;
  }
}

/* Release exclusive access. */
void FSLock::release() {
  tunlink(this->file_name);
}

/* Returns true if the resource is currently locked. */
bool FSLock::is_locked() {
  return JARGON_NAMESPACE(util)::misc::directory_exists(this->file_name);
}

/* Returns class name. */
const char* FSLock::get_class_name() {
  return "FSLock";
}

/* Returns object name. */
const char* FSLock::get_name() const {
  return get_class_name();
}

/* Stringify lock instance. */
std::string FSLock::to_string() {
  return std::string("FSLock@") + this->file_name;
}

JARGON_NAMESPACE_END
