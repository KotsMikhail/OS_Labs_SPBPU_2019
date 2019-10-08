#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <wait.h>

void SigHandler(int signum)
{
  std::cout << "Received signal is " << signum << std::endl;
}

int main() 
{
  signal(SIGINT, SigHandler);
  signal(SIGTERM, SigHandler);
  signal(SIGHUP, SigHandler);
  pid_t pid = fork();
  if (pid == -1)
  {
    perror("Fork failed");
    exit(EXIT_FAILURE);
  }
  else if (pid == 0) // child
  {
    std::cout << "Hello from child with pid = " << getpid() << " and ppid = " << getppid() << std::endl;
    exit(EXIT_SUCCESS);
  }
  else // parent
  {
    std::cout << "Hello from parent with pid = " << getpid() << ", child pid = " << pid << std::endl;
    int status;
    waitpid(pid, &status, 0);
  }
  return EXIT_SUCCESS;
}
