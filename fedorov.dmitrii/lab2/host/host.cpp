#include <iostream>
#include <signal.h>
#include <cstdlib>
#include <unistd.h>
#include <semaphore.h>
#include <stdexcept>

#include "wolf.h"

using namespace std;

int main (int argc, char* argv[]) {
   cout << "Launch host with pid: ";
   cout << getpid() << endl;

   try {
      Wolf& wolf = Wolf::GetInstance(getpid());
      wolf.PrepareGame();
      wolf.StartGame();
   } catch (std::runtime_error &e) {
      cout << e.what() << endl;
      return 1;
   }
   
   return 0;
}
