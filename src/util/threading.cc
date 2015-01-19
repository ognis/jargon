/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#include "threading.h"
#include <cassert>

JARGON_NAMESPACE_BEGIN(util)

#ifndef JARGON_DISABLE_MULTITHREADING
#  if defined(JARGON_DONT_IMPLEMENT_THREAD_MUTEX)
/* Do nothing. */
#    warning "Not implementing any thread mutex. "
#  elif defined(JARGON_HAVE_WINDOWS_H)

/* Represents mutex itself. */
struct MutexHandler::Internal {
  CRITICAL_SECTION mutex;
};

/* Constructor. */
MutexHandler::MutexHandler(const MutexHandler& clone):
  internal(new Internal) {
  InitializeCriticalSection(&internal->mutex);
}

/* Constructor. */
MutexHandler::MutexHandler():
  internal(new Internal) {
  InitializeCriticalSection(&internal->mutex);
}

/* Deconstructor. */
MutexHandler::~MutexHandler() {
  DeleteCriticalSection(&internal->mutex);
  delete internal;
}

/* Lock mutex. */
void MutexHandler::lock() {
  EnterCriticalSection(&internal->mutex);
}

/* Unlock mutex. */
void MutexHandler::unlock() {
  LeaveCriticalSection(&internal->mutex);
}

/* Returns current thread id. */
JARGON_THREAD_ID_T MutexHandler::get_current_id() {
  return GetCurrentThreadId();
}

/* Exit corresponding thread context. */
void MutexHandler::exit(int value){
  ExitThread(value);
}

/* Increment integer atomically. */
int32_t MutexHandler::atomic_increment(JARGON_ATOMIC_INT *integer) {
  return InterlockedIncrement(integer);
}

/* Decrement integer atomically. */
int32_t MutexHandler::atomic_decrement(JARGON_ATOMIC_INT *integer) {
  return InterlockedDecrement(integer);
}

/* Represents shared condition itself. */
class SharedCondition::Internal {
public:
  void* event;
  Internal() {
    event = CreateEventA(NULL, false, false, NULL);
  }
  ~Internal() {
    CloseHandle(event);
  }
};

/* Constructor.  */
SharedCondition::SharedCondition() {
  internal = new Internal;
}

/* Deconstructor. */
SharedCondition::~SharedCondition(){
  delete internal;
}

/* Wait until the assigned condition is satisfied. */
void SharedCondition::wait(MutexHandler* shared_lock){
  shared_lock->unlock();
  dword_t response = WaitForSingleObject(this->internal->_event, 0xFFFFFFFF);
  assert(0x0 == response);
  shared_lock->lock();
}

/* Notify all the other handlers that event is fired. */
void SharedCondition::notify_all() {
  bool response = SetEvent(this->internal->_event);
  assert(response);
}

/* Create new thread. */
JARGON_THREAD_ID_T MutexHandler::create(ThreadFunction* threading, void* arg) {
  return (JARGON_THREAD_ID_T) ::_beginthread(threading, 0, arg);
}

/* Join the other thread context. */
void MutexHandler::join(JARGON_THREAD_ID_T id){
  WaitForSingleObject((void*)id, 0xFFFFFFFF);
}

#  elif defined(JARGON_HAVE_PTHREAD_H)
#    ifndef _REENTRANT
#      error you need to compile with _REENTRANT defined since this uses threads/
#    endif
#    ifdef JARGON_HAVE_PTHREAD_MUTEX_RECURSIVE
bool mutex_pthread_attr_initd=false;
pthread_mutexattr_t mutex_thread_attr;
#    endif

/* Represents mutex itself. */
struct MutexHandler::Internal {
  pthread_mutex_t mutex;
#    ifndef JARGON_HAVE_PTHREAD_MUTEX_RECURSIVE
  pthread_t lock_owner;
  unsigned int lock_count;
#    endif
};

/* Constructor. */
MutexHandler::MutexHandler(const MutexHandler& /* clone */):
  internal(new Internal) {
#    ifdef JARGON_HAVE_PTHREAD_MUTEX_RECURSIVE
  pthread_mutex_init(&internal->mutex, &mutex_thread_attr);
#    else
  pthread_mutex_init(&this->internal->mutex, 0);
  internal->lock_count=0;
  internal->lock_owner=0;
#    endif
}

/* Constructor. */
MutexHandler::MutexHandler():
  internal(new Internal) {
#    ifdef JARGON_HAVE_PTHREAD_MUTEX_RECURSIVE
  if (mutex_pthread_attr_initd == false) {
    pthread_mutexattr_init(&mutex_thread_attr);
    pthread_mutexattr_settype(&mutex_thread_attr, PTHREAD_MUTEX_RECURSIVE);
    mutex_pthread_attr_initd = true;
  }
  pthread_mutex_init(&internal->mutex, &mutex_thread_attr);
#    else
  pthread_mutex_init(&internal->mutex, 0);
  internal->lock_count=0;
  internal->lock_owner=0;
#    endif
}

/* Deconstructor. */
MutexHandler::~MutexHandler() {
  pthread_mutex_destroy(&internal->mutex);
  delete internal;
}

/* Returns current thread id. */
JARGON_THREAD_ID_T MutexHandler::get_current_id() {
  return pthread_self();
}

/* Increment integer atomically. */
int32_t JARGON_ATOMIC_OPERATIONS::INCREMENT(JARGON_ATOMIC_INT *integer) {
#    ifdef JARGON_HAVE_GCC_ATOMIC_FUNCTIONS
  return __sync_add_and_fetch(integer, 1);
#    else
  SCOPED_LOCK_MUTEX(integer->LOCK);
  return ++integer->VALUE;
#    endif
}

/* Decrement integer atomically. */
int32_t JARGON_ATOMIC_OPERATIONS::DECREMENT(JARGON_ATOMIC_INT *integer) {
#    ifdef JARGON_HAVE_GCC_ATOMIC_FUNCTIONS
  return __sync_sub_and_fetch(integer, 1);
#    else
  SCOPED_LOCK_MUTEX(integer->LOCK);
  return --integer->VALUE;
#    endif
}

/* Create new thread. */
JARGON_THREAD_ID_T MutexHandler::create(ThreadFunction* threading, void* arg) {
  JARGON_THREAD_ID_T result;
  int response = pthread_create(&result, NULL, threading, arg);
  assert(response == 0);
  return result;
}

/* Join the other thread context. */
void MutexHandler::join(JARGON_THREAD_ID_T id){
  pthread_join(id, NULL);
}

/* Lock mutex. */
void MutexHandler::lock() {
#    ifndef JARGON_HAVE_PTHREAD_MUTEX_RECURSIVE
  pthread_t current = pthread_self();
  if (pthread_equal(this->internal->lock_owner, current)) {
    ++this->internal->lock_count;
  } else {
    pthread_mutex_lock(&this->internal->mutex);
    this->internal->lock_owner = current;
    this->internal->lock_count = 1;
  }
#    else
  pthread_mutex_lock(&this->internal->mutex);
#    endif
}

/* Unlock mutex. */
void MutexHandler::unlock() {
#    ifndef JARGON_HAVE_PTHREAD_MUTEX_RECURSIVE
  --this->internal->lock_count;
  if (this->internal->lock_count == 0) {
    this->internal->lock_owner = 0;
    pthread_mutex_unlock(&this->internal->mutex);
  }
#    else
  pthread_mutex_unlock(&this->internal->mutex);
#    endif
}

/* Represents shared condition itself. */
struct SharedCondition::Internal {
  pthread_cond_t condition;
  Internal() {
    pthread_cond_init(&this->condition, NULL);
  }
  ~Internal() {
    pthread_cond_destroy(&this->condition);
  }
};

/* Constructor. */
SharedCondition::SharedCondition() {
  this->internal = new Internal;
}

/* Deconstructor. */
SharedCondition::~SharedCondition() {
  delete this->internal;
}

/* Wait until the assigned condition is satisfied. */
void SharedCondition::wait(MutexHandler* shared_lock) {
  int response = 0;
  response = pthread_cond_wait(&this->internal->condition, &shared_lock->internal->mutex);
  assert(response == 0);
}

/* Notify all the other handlers that event is fired. */
void SharedCondition::notify_all() {
   int response = 0;
   response = pthread_cond_broadcast(&this->internal->condition);
   assert(response == 0);
}

#  endif /* thread implementation choice. */

/* Constructor. */
MutexGuard::MutexGuard(const MutexGuard& /*clone*/){
  this->mutex = NULL;
}

/* Constructor. */
MutexGuard::MutexGuard(JARGON_THREAD_MUTEX& mutex) :
  mutex(&mutex) {
  this->mutex->lock();
}

/* Deconstructor. */
MutexGuard::~MutexGuard() {
  this->mutex->unlock();
}

#endif /* JARGON_DISABLE_MULTITHREADING */

JARGON_NAMESPACE_END
