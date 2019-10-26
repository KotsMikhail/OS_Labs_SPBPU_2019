//
// Created by peter on 10/25/19.
//

#include <iostream>
#include <unistd.h>

#include "daemon.h"

int main( int argc, char **argv )
{
  // check CL arguments
  if (argc != 2)
  {
    std::cout << "Usage: ./lab1 path/to/config.conf" << std::endl;
    return EXIT_FAILURE;
  }

  // create child process
  switch (fork())
  {
    case -1:
      perror("Fork failed");
      return EXIT_FAILURE;
    case 0:
      if (!Daemon::start(argv[1]))
        return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

// END OF 'main.cpp' FILE
