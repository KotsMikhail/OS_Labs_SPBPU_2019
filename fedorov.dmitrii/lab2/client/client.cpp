#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <stdexcept>

#include "goat.h"

using namespace std;

int main (int argc, char* argv[]) {
   cout << "Launch client with pid: ";
   cout << getpid() << endl;
   
   if (argc != 2) {
      cout << "Bad arguments to launch client (arg1 - host_pid)" << endl;
      return -1;
   }

   unsigned host_pid = atoi(argv[1]);
   
   cout << "This client works with host (pid): ";
   cout << host_pid << endl;
   
   try {
      Goat& goat = Goat::GetInstance(host_pid);
      goat.PrepareGame();
      goat.StartGame();
   } catch (std::runtime_error &e) {
      cout << e.what() << endl;
      return 1;
   }

   return 0;
}
