/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#include "map.h"
#include "threading.h"
#include <cassert>

JARGON_NAMESPACE_BEGIN(util)

#ifndef JARGON_DISABLE_MULTITHREADING
#  if defined(JARGON_DONT_IMPLEMENT_THREAD_MUTEX)
/* Do nothing. */
#    warning "Not implementing any thread mutex. "
#  elif defined(JARGON_HAVE_WINDOWS_H)

/* Comparator for thread ids. */
class ThreadIdComparator {
 public:
  enum {
    bucket_size = 4,
    min_buckets = 8
  };
  bool operator()(uint64_t left, uint64_t right) const {
    return left < right;
  }
};

/* Initialize thread. */
#    define INIT_THREAD(return) return=true

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

/* Comparator for thread ids. */
class ThreadIdComparator {
 public:
  enum {
    bucket_size = 4,
    min_buckets = 8
  };
  bool operator()(pthread_t left, pthread_t right) const {
    return left < right;
  }
};

/* Initialize thread. */
pthread_key_t pthread_threadlocal_key;
pthread_once_t pthread_threadlocal_key_once = PTHREAD_ONCE_INIT;
#    define INIT_THREAD(return) \
       pthread_once(&pthread_threadlocal_key_once, pthread_threadlocal_make_key);\
       if (pthread_getspecific(pthread_threadlocal_key) == NULL) {\
         pthread_setspecific(pthread_threadlocal_key, (void*)1);\
       }

void pthread_threadlocal_destructor(void* /* handle */) {
  ThreadLocalHandler::unregister();
}

void pthread_threadlocal_make_key() {
  (void) pthread_key_create(&pthread_threadlocal_key, &pthread_threadlocal_destructor);
}

/* Represents mutex itself. */
struct MutexHandler::Internal {
  pthread_mutex_t mutex;
#    ifndef JARGON_HAVE_PTHREAD_MUTEX_RECURSIVE
  pthread_t lock_owner;
  unsigned int lock_count;
#    endif
};

/* Constructor. */
MutexHandler::MutexHandler(const MutexHandler& /* clone */) :
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
MutexHandler::MutexHandler() :
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
void MutexHandler::join(JARGON_THREAD_ID_T id) {
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
MutexGuard::MutexGuard(const MutexGuard& /*clone*/) {
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

#else /* JARGON_DISABLE_MULTITHREADING */

/* Comparator for thread ids. */
class ThreadIdComparator {
 public:
  enum {
    bucket_size = 4,
    min_buckets = 8
  };
  bool operator()(char left, char right) const {
    return left < right;
  }
};

#endif /* JARGON_DISABLE_MULTITHREADING */

/* Set that holds the list of ThreadLocals that this thread has data in. */
class ThreadLocalManager : public std::set<ThreadLocalHandler*> {
 public:
  /* Clear all relating thread local variables. */
  void unregister() {
    for (ThreadLocalManager::iterator iter = begin(); iter != end(); iter++) {
      (*iter)->set_null();
    }
    this->clear();
  }

  /* Add thread local variable to the current thread. */
  void add(ThreadLocalHandler* handler) {
    if (this->end() == this->find(handler)) {
      this->insert(handler);
    }
  }

  /* Remove thread local variable from the current thread. */
  void remove(ThreadLocalHandler* handler) {
    ThreadLocalManager::iterator iter = this->find(handler);
    if (iter != this->end()) {
      this->erase(handler);
    }
  }
};

/* ThreadLocal storage. */
typedef JARGON_NAMESPACE(util)::MultiMap<
    JARGON_THREAD_ID_T,
    ThreadLocalManager*,
    JARGON_NAMESPACE(util)::ThreadIdComparator,
    JARGON_NAMESPACE(util)::Freeable::ConstNullValue<JARGON_THREAD_ID_T>,
    JARGON_NAMESPACE(util)::Freeable::Object<ThreadLocalManager>
    > ThreadLocalStorage;
static ThreadLocalStorage* storage = NULL; 

#ifndef JARGON_DISABLE_MULTITHREADING
static JARGON_THREAD_MUTEX* STORAGE_LOCK = NULL;
#endif

/* Internal implementation class holds context which is shared with relating threads. */
class ThreadLocalHandler::Internal {
 public:
  typedef JARGON_NAMESPACE(util)::ParametrizedSet<
      JARGON_THREAD_ID_T,
      void*,
      JARGON_NAMESPACE(util)::ThreadIdComparator,
      JARGON_NAMESPACE(util)::Freeable::ConstNullValue<JARGON_THREAD_ID_T>,
      JARGON_NAMESPACE(util)::Freeable::ConstNullValue<void*>
      > LocalContext;
  LocalContext context;
  DEFINE_MUTEX(CONTEXT_LOCK);
  AbstractDeleter* deleter;

  /* Constructor. */
  Internal (AbstractDeleter* deleter) :
      context(false, false) {
    this->deleter = deleter;
  }

  /* Deconstructor. */
  ~Internal() {
    LocalContext::iterator iter = this->context.begin();
    while (iter != this->context.end()) {
      void* value = iter->second;
      this->context.remove(iter);
      this->deleter->do_delete(value);
      iter = this->context.begin();
    }
    delete deleter;
  }
};

/* Constructor. */
ThreadLocalHandler::ThreadLocalHandler(
    JARGON_NAMESPACE(util)::AbstractDeleter* deleter) :
    internal(new Internal(deleter)) {}

/* Deconstructor. */
ThreadLocalHandler::~ThreadLocalHandler() {
  this->set_null();
  unregister();
  remove(this);
  delete this->internal;
}

/* Returns thread local data which is binded with corresponding thread. */
void* ThreadLocalHandler::get() {
  SCOPED_LOCK_MUTEX(internal->CONTEXT_LOCK);
  return this->internal->context.get(JARGON_CURRENT_THREAD_ID);
}

/* Sets thread local data to null which is binded with corresponding thread. */
void ThreadLocalHandler::set_null() {
  JARGON_THREAD_ID_T id = JARGON_CURRENT_THREAD_ID;
  SCOPED_LOCK_MUTEX(this->internal->CONTEXT_LOCK);
  Internal::LocalContext::iterator iter = this->internal->context.find(id);
  if (iter != this->internal->context.end()) {
    void* value = iter->second;
    this->internal->context.remove(iter);
    this->internal->deleter->do_delete(value);
  }
}

/* Sets thread local data which is binded with corresponding thread. */
void ThreadLocalHandler::set(void* local) {
  if (local == NULL) {
    this->set_null();
    return;
  }

  bool ensure_init;
  INIT_THREAD(ensure_init);
  assert(ensure_init);
  JARGON_THREAD_ID_T id = JARGON_CURRENT_THREAD_ID;

  {
#ifndef JARGON_DISABLE_MULTITHREADING
    if (STORAGE_LOCK == NULL) {
      STORAGE_LOCK = new JARGON_THREAD_MUTEX;
    }
    SCOPED_LOCK_MUTEX(*STORAGE_LOCK);
#endif

    if (storage == NULL) {
      storage = new ThreadLocalStorage(false, true);
    }

    ThreadLocalManager* manager = storage->get(id);
    if (manager == NULL) {
      manager = new ThreadLocalManager;
      storage->insert(std::pair<
          const JARGON_THREAD_ID_T,
	  ThreadLocalManager*>(id, manager));
    }
    manager->add(this);
  }

  {
    SCOPED_LOCK_MUTEX(this->internal->CONTEXT_LOCK);
    Internal::LocalContext::iterator iter = this->internal->context.find(id);
    if (iter != this->internal->context.end()) {
      void* value = iter->second;
      this->internal->context.remove(iter);
      this->internal->deleter->do_delete(value);
    }
    
    if (local != NULL) {
      this->internal->context.put(id, local);
    }
  }
}

/* Unregister thread local variables. */
void ThreadLocalHandler::unregister() {
  if (storage == NULL ) {
    return;
  }
  JARGON_THREAD_ID_T id = JARGON_CURRENT_THREAD_ID;
  SCOPED_LOCK_MUTEX (*STORAGE_LOCK);

  ThreadLocalStorage::iterator iter = storage->find(id);
  if (iter != storage->end()) {
    ThreadLocalManager* manager = iter->second;
    manager->unregister();
    storage->remove(iter);
  }
}

/* Remove thread local variable from the current thread. */
void ThreadLocalHandler::remove(ThreadLocalHandler* local) {
  if (storage == NULL) {
    return;
  }
  SCOPED_LOCK_MUTEX(*STORAGE_LOCK);
  for(ThreadLocalStorage::iterator iter = storage->begin(); iter != storage->end(); iter++) {
    ThreadLocalManager* manager = iter->second;
    manager->remove(local);
  }
}

void ThreadLocalHandler::shutdown() {
#ifndef JARGON_DISABLE_MULTITHREADING
  JARGON_FREE(STORAGE_LOCK);
#endif
  JARGON_FREE(storage);
}

JARGON_NAMESPACE_END
