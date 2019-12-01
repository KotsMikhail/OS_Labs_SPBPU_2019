#include <iostream>
#include <signal.h>
#include <cstdlib>
#include <unistd.h>
#include <semaphore.h>

#include "wolf.h"

using namespace std;

int main (int argc, char* argv[]) {
   cout << "Launch host with pid: ";
   cout << getpid() << endl;

   Wolf* wolf = Wolf::GetInstance(getpid());
   
   if (wolf != nullptr) {
      wolf->PrepareGame();
      wolf->StartGame();
      delete wolf;
      return 0;
   }

   return 1;
}
