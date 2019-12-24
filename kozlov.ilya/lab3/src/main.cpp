#include "test/tester.h"

int main(int argc, char** argv)
{
  if (!Logger::init())
  {
    return -1;
  }
  Tester<int> tester;
  if (argc != 2)
  {
    std::cout << "Usage: ./tests path/to/test.cfg" << std::endl;
    exit(EXIT_FAILURE);
  }
  tester.loadConfig(argv[1]);
  tester.runTests();
  Logger::destroy();
  return 0;
}