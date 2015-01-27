/*
 * Test code.
 */
#include <ctime>
#include <iostream>
#include <locale>
#include <string>
#include "../src/shared/monolithic.h"
#include "../src/util/object.h"
#include "../src/util/algorithm.h"

#define RANGE 100

typedef JARGON_NAMESPACE(util)::Ordered::Int32 Int32;

inline void set_seed() {
  srand((unsigned int)time(NULL));
}

inline int random(int range) {
  return rand() % range + 1;
}

inline void debug(Int32* array[], const char* message) {
  std::wcout << message <<std::endl;
  for (int i = 0; i < RANGE; i++) {
    std::wcout << array[i]->get_value() << TLITERAL(" ");
  }
  std::wcout << std::endl;
}

int main(int argc, char* argv[]) {
  std::ios_base::sync_with_stdio(false);
  std::locale default_loc("");
  std::locale::global(default_loc);
  std::locale ctype_default(std::locale::classic(), default_loc, std::locale::ctype);
  std::wcout.imbue(ctype_default);
  std::wcin.imbue(ctype_default);

  Int32* array[RANGE];
  set_seed();
  for (int i = 0; i < RANGE; i++) {
    array[i] = new Int32(random(RANGE));
  }

  debug(array, "initial array");
  JARGON_NAMESPACE(util)::algorithm<Int32*>::msort(array, RANGE, 0, RANGE);
  debug(array, "sorted array");

  return 0;
}
