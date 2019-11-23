#include "goat.h"

#include <iostream>
#include <semaphore.h>
#include <cstdlib>
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include <random>
#include <signal.h>
#include <time.h>

#include "../connections/conn.h"
#include "../support/message_types.h"

static const int g_timeout = 5;


Goat::Goat (int host_pid_) {
   host_pid = host_pid_;
   status = 0;
   
   conn = new Conn(host_pid, false);
   sem_host = sem_open(std::string("host_" + std::to_string(host_pid)).c_str(), 0);
   sem_client = sem_open(std::string("client_" + std::to_string(host_pid)).c_str(), 0);
   signal(SIGTERM, OnSignalRecieve);

   std::cout << "Goat constructed" << std::endl; 
}


void Goat::Terminate () {
   kill(host_pid, SIGUSR2);
   delete conn;
   std::cout << "Goat terminated" << std::endl; 
} 


Goat& Goat::GetInstance (int host_pid)
{
   static Goat goat = Goat(host_pid);  
   if (goat.sem_host == SEM_FAILED || goat.sem_client == SEM_FAILED) {
      exit(EXIT_FAILURE);
   }
   return goat;
}


void Goat::PrepareGame ()
{
   kill(host_pid, SIGUSR1);
}


void Goat::StartGame () {
   std::cout << "START GAME AS GOAT" << std::endl;
   
   while (true) {
      if (!SemWait(sem_client)) {
         Terminate();
         return;
      }

      Msg msg;
      memset(&msg, 0, sizeof(msg));     

      if (!conn->Read(&msg, sizeof(msg))) {
         std::cout << "Error in conn->read: terminate client" << std::endl;
         Terminate();
         return;
      }
      
      std::cout << "___________GAME_STEP___________" << std::endl;
      status = msg.data;

      int cur_val = GenerateValue();
      std::cout << "Goat: number is " << cur_val << std::endl;
      
      msg.type = 2;
      msg.data = cur_val;
      
      if (!conn->Write(&msg, sizeof(msg))) {
         std::cout << "Error in conn->write: terminate client" << std::endl;
         Terminate();
         return;
      }

      if (!SemSignal(sem_host)) {
         Terminate();
         return;
      }
   }
}


int Goat::GenerateValue () {   
   std::random_device rd;
   std::mt19937 gen(rd());
   if (!IsAlive()) {
      std::uniform_int_distribution<int> dis(1, 100);
      return dis(gen);
   } else {
      std::uniform_int_distribution<int> dis(1, 50);
      return dis(gen); 
   }
}


bool Goat::SemWait (sem_t* sem) {

  if (Conn::GetType() == "conn_fifo") {
      return true;
   }

   struct timespec ts;
   clock_gettime(CLOCK_REALTIME, &ts);
   ts.tv_sec += g_timeout;
   if (sem_timedwait(sem, &ts) == -1) {
      perror("sem_timewait() ");
      return false;
   }

   return true;
}



bool Goat::SemSignal (sem_t* sem) {
  if (Conn::GetType() == "conn_fifo") {
      return true;
   }

   if (sem_post(sem) == -1) {
      perror("sem_post() ");
      return false;
   }

   return true;
}


void Goat::OnSignalRecieve (int sig) {
   switch (sig) {
      case SIGTERM:
      {
         std::cout << "Terminate client" << std::endl; 
         Goat& goat = Goat::GetInstance(0);
         goat.Terminate();
         exit(EXIT_SUCCESS);
         break;
      }
      default:
      {
         std::cout << "Unknown signal recieve: continue work" << std::endl; 
         break;
      }
   }
}

