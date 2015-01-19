/*
 * Test code.
 */
#include <iostream>
#include <locale>
#include "../src/shared/monolithic.h"
#include "../src/util/stringbuffer.h"

int main(int argc, char* argv[]) {
  std::ios_base::sync_with_stdio(false);
  std::locale default_loc("");
  std::locale::global(default_loc);
  std::locale ctype_default(std::locale::classic(), default_loc, std::locale::ctype);
  std::wcout.imbue(ctype_default);
  std::wcin.imbue(ctype_default);

  StringBuffer* buffer = new StringBuffer;
  buffer->append_string(L"あいうえお");
  buffer->append_string(L" ");
  buffer->append_string(L"かきくけこ");

  std::wcout << L"TEST" << std::endl;
  std::wcout << L"テスト" << std::endl;
  std::wcout << buffer->to_string() << std::endl;

  return 0;
}
