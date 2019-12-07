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
#include <stdexcept>

#include "../connections/conn.h"
#include "../support/message_types.h"
#include "../support/game_consts.h"

static const int g_timeout = 5;


Goat::Goat (int host_pid_)
   : conn(host_pid_, false)
{
   std::string sem_host_name = std::string("host_" + std::to_string(host_pid));
   std::string sem_client_name = std::string("client_" + std::to_string(host_pid));

   host_pid = host_pid_;
   status = 0;
   
   sem_host = sem_open(sem_host_name.c_str(), 0);
   if (sem_host == SEM_FAILED) {
      throw std::runtime_error("sem_host wasn't opened");
   }

   sem_client = sem_open(sem_client_name.c_str(), 0);
   if (sem_client == SEM_FAILED) {
       sem_close(sem_host);
       throw std::runtime_error("sem_client wasn't opened");
   }

   signal(SIGTERM, OnSignalRecieve);
   signal(SIGINT, OnSignalRecieve);

   std::cout << "Goat constructed" << std::endl; 
}


Goat::~Goat () {
   kill(host_pid, SIGTERM);

   if (sem_host != SEM_FAILED) {
      sem_close(sem_host);
   }
   
   if (sem_client != SEM_FAILED) {
      sem_close(sem_client);
   }

   std::cout << "Goat terminated" << std::endl; 
}



Goat& Goat::GetInstance (int host_pid)
{
   static Goat goat(host_pid);  
   return goat;
}


void Goat::PrepareGame ()
{
   kill(host_pid, SIGUSR1);
}


void Goat::StartGame () {
   std::cout << "START GAME AS GOAT" << std::endl;

   std::cout << "___________GAME_STEP___________" << std::endl;
   if (!GenAndWriteValue()) {
      return;
   }

   if (!SemSignal(sem_host)) {
      return;
   }
   
   while (true) {
      std::cout << "___________GAME_STEP___________" << std::endl;

      if (!SemWait(sem_client)) {
         return;
      }

      Msg msg;
      memset(&msg, 0, sizeof(msg));     

      if (!conn.Read(&msg, sizeof(msg))) {
         return;
      }      

      status = msg.data;

      if (!GenAndWriteValue()) {
         return;
      }
      if (!SemSignal(sem_host)) {
         return;
      }
   }
}


int Goat::GenerateValue () {   
   std::random_device rd;
   std::mt19937 gen(rd());
   if (IsAlive()) {
      std::uniform_int_distribution<int> dis(g_min_goat_val_alive, g_max_goat_val_alive);
      return dis(gen);
   } else {
      std::uniform_int_distribution<int> dis(g_min_goat_val_dead, g_max_goat_val_dead);
      return dis(gen); 
   }
}


bool Goat::GenAndWriteValue() {
   Msg msg;
   int cur_val = GenerateValue();
   std::cout << "Goat: number is " << cur_val << std::endl;
      
   msg.type = 2;
   msg.data = cur_val;
      
   if (!conn.Write(&msg, sizeof(msg))) {
      return false;
   }
   return true;
}


bool Goat::SemWait (sem_t* sem) {
   if (sem_wait(sem) == -1) {
      perror("sem_timewait() ");
      return false;
   }

   return true;
}


bool Goat::SemSignal (sem_t* sem) {
   if (sem_post(sem) == -1) {
      perror("sem_post() ");
      return false;
   }

   return true;
}


void Goat::OnSignalRecieve (int sig) {
   switch (sig) {
      case SIGTERM:
      case SIGINT:
      {
         std::cout << "Terminate client (signal)" << std::endl; 
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

