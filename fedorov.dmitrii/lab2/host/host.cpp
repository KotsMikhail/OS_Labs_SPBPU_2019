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

   Wolf& wolf = Wolf::GetInstance(getpid());
   wolf.PrepareGame();
   wolf.StartGame();

   return 0;
}
