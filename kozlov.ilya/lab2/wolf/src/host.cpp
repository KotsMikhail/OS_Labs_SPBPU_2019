#include <iostream>
#include "wolf.h"

int main()
{
  std::cout << "Starting wolf..." << std::endl;
  Wolf wolf = Wolf::GetInstance();
  if (Wolf::last_err == 0)
  {
    wolf.Start();
  }
  return 0;
}