#include <iostream>
#include <unistd.h>
#include <wait.h>

#include "daemon.h"

int main(int argc, char** argv)
{
  if (argc != 2)
  {
    std::cout << "Usage: ./lab1 path/to/config.conf" << std::endl;
    exit(EXIT_FAILURE);
  }

  pid_t pid = fork();
  if (pid == -1)
  {
    perror("Fork failed");
    exit(EXIT_FAILURE);
  }
  else if (pid == 0) // child
  {
    std::cout << "Hello from child with pid = " << getpid() << " and ppid = " << getppid() << std::endl;
    if (Daemon::Init(argv[1]))
    {
      Daemon::Start();
    }
    else
    {
      exit(EXIT_FAILURE);
    }
  }
  else // parent
  {
    std::cout << "Hello from parent with pid = " << getpid() << ", child pid = " << pid << std::endl;
    //int status;
    //waitpid(pid, &status, 0);
  }
  return EXIT_SUCCESS;
}
