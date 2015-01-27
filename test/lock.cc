/*
 * Test code.
 */
#include <ctime>
#include <iostream>
#include <locale>
#include <string>
#include "../src/shared/monolithic.h"
#include "../src/directory/lock.h"

int main(int argc, char* argv[]) {
  std::ios_base::sync_with_stdio(false);
  std::locale default_loc("");
  std::locale::global(default_loc);
  std::locale ctype_default(std::locale::classic(), default_loc, std::locale::ctype);
  std::wcout.imbue(ctype_default);
  std::wcin.imbue(ctype_default);

  std::wcout << "create new lock instance." <<std::endl;
  typedef JARGON_NAMESPACE(directory)::FSLock FSLock;
  FSLock* lock = new FSLock("./test", "test", 0644);

  std::wcout << "obtain locking." <<std::endl;
  lock->obtain();
  std::wcout << "is locked: " << lock->is_locked() <<std::endl;
  std::wcout << "release locking." <<std::endl;
  lock->release();
  std::wcout << "is locked: " << lock->is_locked() <<std::endl;

  return 0;
}
