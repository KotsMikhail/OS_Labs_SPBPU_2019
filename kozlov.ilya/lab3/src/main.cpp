#include "test/tester.h"

int main()
{
  Logger::init();
  Tester<int> tester;
  tester.LoadConfig("");
  tester.runTests();
  return 0;
}