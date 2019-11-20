#include <iostream>
#include "goat.h"

int main(int argc, char* argv[])
{
  if (argc != 2)
  {
    std::cout << "Usage: ./client_<connection_type> <host pid>" << std::endl;
    return 1;
  }
  int pid;
  try
  {
    pid = std::stoi(argv[1]);
  }
  catch (std::exception &e)
  {
    std::cout << "ERROR: can't get int pid from argument" << std::endl;
    return 1;
  }
  Goat& goat = Goat::GetInstance(pid);
  if (goat.OpenConnection())
  {
    goat.Start();
  }
  return 0;
}