/*
 * Copyright (C) 2014- Shingo OKAWA.
 */
#ifndef jargon_util_threading_h
#define jargon_util_threading_h

#include "object.h"
#include "../shared/monolithic.h"

/* Defines platform-specific mutex. */
#ifdef JARGON_HAVE_WINDOWS_H
#  include <windows.h>
#  define JARGON_MUTEX_T HANDLE
#  define JARGON_MUTEX_INIT(mutex) mutex = CreateMutex(NULL, FALSE, NULL)
#  define JARGON_MUTEX_LOCK(mutex) WaitForSingleObject(*mutex, INFINITE)
#  define JARGON_MUTEX_UNLOCK(mutex) ReleaseMutex(*mutex)
#  define JARGON_MUTEX_DESTROY(mutex) CloseHandle(mutex)
#elif defined(JARGON_USE_BOOST_THREADING)
#  include <boost/thread/mutex.hpp>
#  define JARGON_MUTEX_T boost::mutex
#  define JARGON_MUTEX_INIT(mutex)
#  define JARGON_MUTEX_LOCK(mutex) (*mutex).lock()
#  define JARGON_MUTEX_UNLOCK(mutex) (*mutex).unlock()
#  define JARGON_MUTEX_DESTROY(mutex) mutex.unlock()
#elif defined(JARGON_HAVE_PTHREAD_H)
#  include <pthread.h>
#  define JARGON_MUTEX_T pthread_mutex_t
#  define JARGON_MUTEX_INIT(mutex) pthread_mutex_init(&mutex, NULL)
#  define JARGON_MUTEX_LOCK(mutex) pthread_mutex_lock(mutex)
#  define JARGON_MUTEX_UNLOCK(mutex) pthread_mutex_unlock(mutex)
#  define JARGON_MUTEX_DESTROY(mutex) pthread_mutex_destroy(&mutex)
#else
#  define JARGON_DISABLE_MULTITHREADING
#endif

JARGON_NAMESPACE_BEGIN(util)

/* Class to compare thread ids. */
class ThreadIdComparator;

/* Defines platform-specific mutext handlers. */
#if defined(JARGON_DISABLE_MULTITHREADING)
#  define SCOPED_LOCK_MUTEX(mutex)
#  define DEFINE_MUTEX(x)
#  define DEFINE_MUTABLE_MUTEX(x)
#  define DEFINE_CONDITION(x)
#  define DEFINE_STATIC_MUTEX(x)
#  define CONDITION_WAIT(mutex, condition)
#  define CONDITION_NOTIFY_ALL(condition)
#  define JARGON_CURRENT_THREAD_ID 1
#  define JARGON_THREAD_ID_T int
#  define JARGON_THREAD_FUNC(function, args) int function(void* args)
#  define JARGON_THREAD_FUNC_RETURN(value) return (int) value
#  define JARGON_THREAD_CREATE(function, args) (*function)(args)
#  define JARGON_THREAD_JOIN(value)
#  define JARGON_THREAD_MUTEX void*
#  define JARGON_ATOMIC_INC(integer) (++(*integer))
#  define JARGON_ATOMIC_DEC(integer) (--(*integer))
#  define JARGON_ATOMIC_INT int
#else
#  if defined(JARGON_DONT_IMPLEMENT_THREAD_MUTEX)
/* Do nothing. */
#  else

/* Class to represents mutex guard. */
class MutexGuard;

/* Defines macros for pthread. */
#    if defined(JARGON_HAVE_PTHREAD_H)
#      define JARGON_THREAD_ID_T pthread_t
#      define JARGON_THREAD_FUNC(function, args) void* function(void* args)
#      define JARGON_THREAD_FUNC_RETURN(value) return (void*) value

/* Defines thread-start function. */
typedef void* (ThreadFunction)(void* args);

/* Class to represent mutex-driven thread. */
class MutexHandler {
 public:
  struct Internal;
  Internal* internal;
  MutexHandler(const MutexHandler& clone);
  MutexHandler();
  ~MutexHandler();
  void lock();
  void unlock();
  void wait(MutexHandler* shared_lock);
  void notify_all();
  static JARGON_THREAD_ID_T get_current_id();
  static JARGON_THREAD_ID_T create(ThreadFunction* threading, void* arg);
  static void join(JARGON_THREAD_ID_T id);
};

/* Class to represent thread shared condition. */
class SharedCondition{
 private:
  class Internal;
  Internal* internal;
 public:
  SharedCondition();
  ~SharedCondition();
  void wait(MutexHandler* shared_lock);
  void notify_all();
};

/* Defines platform-specific atomic operations. */
#      ifdef JARGON_HAVE_GCC_ATOMIC_FUNCTIONS
#        define JARGON_ATOMIC_INT uint32_t
#        define JARGON_ATOMIC_INT_SET(x,v) x=v
#        define JARGON_ATOMIC_INT_GET(x) x
#      else

class JARGON_ATOMIC_INT_IMPL {
 public:
  int VALUE;
  JARGON_NAMESPACE(util)::MutexHandler LOCK;
};

#        define JARGON_ATOMIC_INT JARGON_NAMESPACE(util)::JARGON_ATOMIC_INT_IMPL
#        define JARGON_ATOMIC_INT_SET(x,v) x.VALUE=v
#        define JARGON_ATOMIC_INT_GET(x) x.VALUE
#      endif

class JARGON_ATOMIC_OPERATIONS {
 public:
  static int32_t INCREMENT(JARGON_ATOMIC_INT* integer);
  static int32_t DECREMENT(JARGON_ATOMIC_INT* integer);
};

#      define JARGON_ATOMIC_INC(integer) JARGON_NAMESPACE(util)::JARGON_ATOMIC_OPERATIONS::INCREMENT(integer)
#      define JARGON_ATOMIC_DEC(integer) JARGON_NAMESPACE(util)::JARGON_ATOMIC_OPERATIONS::DECREMENT(integer)

/* Defines macros for win32 thread. */
#    elif defined(JARGON_HAVE_WINDOWS_H)
#      define JARGON_THREAD_ID_T uint64_t
#      define JARGON_THREAD_FUNC(function, args) void __stdcall function(void* args)
#      define JARGON_THREAD_FUNC_RETURN(value) JARGON_NAMESPACE(util)::MutexHandler::exit(value)
#      define JARGON_ATOMIC_INC(integer) JARGON_NAMESPACE(util)::MutexHandler::atomic_increment(integer)
#      define JARGON_ATOMIC_DEC(integer) JARGON_NAMESPACE(util)::MutexHandler::atomic_decrement(integer)
#      ifdef _M_X64
#        define JARGON_ATOMIC_INT long long
#      else
#        define JARGON_ATOMIC_INT long
#      endif
#      define JARGON_ATOMIC_INT_SET(x,v) x=v
#      define JARGON_ATOMIC_INT_GET(x) x

/* Defines thread-start function. */
typedef void* (ThreadFunction)(void* args);

/* Class to represent mutex-driven thread. */
class MutexHandler {
 private:
  struct Internal;
  Internal* internal;
 public:
  MutexHandler(const MutexHandler& clone);
  MutexHandler();
  ~MutexHandler();
  void lock();
  void unlock();
  static void exit(int ret);
  static JARGON_THREAD_ID_T get_current_id();
  static JARGON_THREAD_ID_T create(ThreadFunction* threading, void* arg);
  static void join(JARGON_THREAD_ID_T id);
  static int32_t atomic_increment(JARGON_ATOMIC_INT* integer);
  static int32_t atomic_decrement(JARGON_ATOMIC_INT* integer);
};

/* Class to represent thread shared condition. */
class SharedCondition {
 private:
  class Internal;
  Internal* internal;
 public:
  SharedCondition();
  ~SharedCondition();
  void wait(MutexHandler* shared_lock);
  void notify_all();
};

#    else
#      error A valid thread library was not found
#    endif //mutex types
#    define JARGON_THREAD_CREATE(func, arg) JARGON_NAMESPACE(util)::MutexHandler::create(func, arg)
#    define JARGON_THREAD_JOIN(id) JARGON_NAMESPACE(util)::MutexHandler::join(id)
#    define JARGON_CURRENT_THREAD_ID JARGON_NAMESPACE(util)::MutexHandler::get_current_id()
#    define JARGON_THREAD_MUTEX JARGON_NAMESPACE(util)::MutexHandler
#    define JARGON_THREAD_CONDITION JARGON_NAMESPACE(util)::SharedCondition
#  endif //don't implement

/* Class to represent mutex guard. */
class MutexGuard {
 private:
  JARGON_THREAD_MUTEX* mutex;
  MutexGuard(const MutexGuard& clone);
 public:
  MutexGuard(JARGON_THREAD_MUTEX& mutex);
  ~MutexGuard();
};

#  define SCOPED_LOCK_MUTEX(mutex) JARGON_NAMESPACE(util)::MutexGuard mutex_guard(mutex)
#  define DEFINE_MUTEX(mutex) JARGON_THREAD_MUTEX mutex
#  define DEFINE_CONDITION(condition) JARGON_THREAD_CONDITION condition
#  define DEFINE_MUTABLE_MUTEX(mutex) mutable JARGON_THREAD_MUTEX mutex
#  define DEFINE_STATIC_MUTEX(mutex) static JARGON_THREADMUTEX mutex
#  define CONDITION_WAIT(mutex, condition) condition.wait(&mutex)
#  define CONDITION_NOTIFY_ALL(condition) condition.notify_all()
#endif

/* Class holds context which is shared with relating threads. */
class ThreadLocalHandler {
 public:
  typedef void Callback(bool startup);
  static void remove(ThreadLocalHandler* local);
  static void unregister();
  static void shutdown();
  void* get();
  void set(void* local);
  void set_null();
  ThreadLocalHandler(JARGON_NAMESPACE(util)::AbstractDeleter*);
  virtual ~ThreadLocalHandler();
 private:
  class Internal;
  Internal* internal;
};

/* Templatized thread specific data holder. */
template<typename Type, typename Deleter>
class ThreadLocal : public ThreadLocalHandler {
 public:
  ThreadLocal() : ThreadLocalHandler(new Deleter) {}
  virtual ~ThreadLocal() {}
  Type get() {return (Type) ThreadLocalHandler::get();}
  void set(Type local) {ThreadLocalHandler::set((Type) local);}
};

JARGON_NAMESPACE_END

#endif /* jargon_util_threading_h */
