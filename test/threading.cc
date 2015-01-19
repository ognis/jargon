/*
 * Test code.
 */
#include <iostream>
#include <locale>
#include <string>
#include "../src/shared/monolithic.h"
#include "../src/util/threading.h"

DEFINE_MUTEX(mutex_one);
DEFINE_CONDITION(condition_one);
DEFINE_MUTEX(mutex_two);
DEFINE_CONDITION(condition_two);

JARGON_THREAD_FUNC(threading, text) {
  std::wcout << (tchar_t*) text << std::endl;
  CONDITION_NOTIFY_ALL(condition_one);
  SCOPED_LOCK_MUTEX(mutex_two);
  CONDITION_WAIT(mutex_two, condition_two);
}

int main(int argc, char* argv[]) {
  std::ios_base::sync_with_stdio(false);
  std::locale default_loc("");
  std::locale::global(default_loc);
  std::locale ctype_default(std::locale::classic(), default_loc, std::locale::ctype);
  std::wcout.imbue(ctype_default);
  std::wcin.imbue(ctype_default);

  {
    const tchar_t* text = TLITERAL("あいうえお");
    JARGON_THREAD_ID_T thread = JARGON_THREAD_CREATE(threading, (void*)text);
    SCOPED_LOCK_MUTEX(mutex_one);
    CONDITION_WAIT(mutex_one, condition_one);
    CONDITION_NOTIFY_ALL(condition_two);
    JARGON_THREAD_JOIN(thread);
  }

  {
    const tchar_t* text = TLITERAL("かきくけこ");
    JARGON_THREAD_ID_T thread = JARGON_THREAD_CREATE(threading, (void*)text);
    SCOPED_LOCK_MUTEX(mutex_one);
    CONDITION_WAIT(mutex_one, condition_one);
    CONDITION_NOTIFY_ALL(condition_two);
    JARGON_THREAD_JOIN(thread);
  }

  return 0;
}
