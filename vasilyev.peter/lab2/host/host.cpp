//
// Created by peter on 11/22/19.
//

#include <stdexcept>
#include <iostream>

#include "host_class.h"

int main( int argc, char **argv )
{
  try
  {
    Host &host = Host::getInstance();
    host.start();
  }
  catch (std::exception &e)
  {
    std::cout << "ERROR: " << e.what() << ", errno = " << strerror(errno) << std::endl;
    return 1;
  }

  return 0;
}
