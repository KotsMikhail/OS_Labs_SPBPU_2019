#include "wolf.h"

#include <iostream>
#include <cstdlib>
#include <semaphore.h>
#include <string>
#include <cstring>
#include <fcntl.h>
#include <random>
#include <time.h>
#include <signal.h>
#include <unistd.h>

#include "../connections/conn.h"
#include "../support/message_types.h"

static const int g_timeout = 5;


Wolf::Wolf (int host_pid) {
   is_client_connected = false;
   client_pid = 0;

   conn = new Conn(host_pid, true);
   sem_host = sem_open(std::string("host_" + std::to_string(host_pid)).c_str(), O_CREAT, 0666, 0);
   sem_client = sem_open(std::string("client_" + std::to_string(host_pid)).c_str(), O_CREAT, 0666, 0);

   std::cout << "Wolf constructed" << std::endl; 
} 


void Wolf::Terminate () {
   if (is_client_connected) {
      kill(client_pid, SIGTERM);
   }
   delete conn;
   std::cout << "Wolf terminated" << std::endl; 
} 


Wolf& Wolf::GetInstance (int host_pid)
{
   static Wolf wolf = Wolf(host_pid);
   if (wolf.sem_host == SEM_FAILED || wolf.sem_client == SEM_FAILED) {
      exit(EXIT_FAILURE);
   }
   return wolf;
}


void Wolf::PrepareGame () {
   struct sigaction sa;
   memset(&sa, 0, sizeof(sa));
   sa.sa_flags = SA_SIGINFO;
   sa.sa_sigaction = HandleSignal;
   sigaction(SIGTERM, &sa, nullptr);
   sigaction(SIGUSR1, &sa, nullptr);
   sigaction(SIGUSR2, &sa, nullptr);
   pause();
}


void Wolf::StartGame () {
   std::cout << "START GAME AS WOLF" << std::endl;
   bool is_first_step = true;

   while (true) {
      Msg msg;

      if (!is_first_step) {
         if (!SemWait(sem_host)) {
            Terminate();
            return;
         }      

         if (!conn->Read(&msg, sizeof(msg))) {
            std::cout << "Error in conn->read: terminate host" << std::endl;
            exit(EXIT_FAILURE);
         }

         std::cout << "___________GAME_STEP___________" << std::endl;
         int cur_val = GetValue();
         std::cout << "Wolf: num is " << cur_val << std::endl;
  
         int goat_val = msg.data;
         std::cout << "Goat num: " << goat_val << std::endl;   
     
         if (goat_state == 0 && abs(goat_val - cur_val) > 70) {
            goat_state++;
         } else if (goat_state != 0) {
            if (goat_state == 1 && abs(goat_val - cur_val) > 20) {
               goat_state++;
            } else {
               goat_state--;
            }
         }
      }

      is_first_step = false;

      if (goat_state == 0) {
         std::cout << "Goat: I am alive with full hp" << std::endl;
      } else if (goat_state == 1) {
         std::cout << "Goat: I am dead, but there is a chance to be revived" << std::endl;
      } else if (goat_state == 2) {
         std::cout << "Goat: I am dead" << std::endl;
      }   

      msg.type = 1;
      msg.data = goat_state;
      if (!conn->Write(&msg, sizeof(msg))) {
         std::cout << "Error in conn->write: terminate host" << std::endl;
         Terminate();
         exit(EXIT_FAILURE);
      }

      if (goat_state == 2) {
         std::cout << "_Goat__is__dead______END_GAME__" << std::endl;   
         Terminate();
         return;
      }

      if (!SemSignal(sem_client)) {
         return;
      }
   }
}


int Wolf::GetValue ()
{
   int res;
   std::cout << "Enter number from 1 to 100" << std::endl;

   do {
      std::cin >> res;
   } while (res < 1 || res > 100);
   return res;
}


bool Wolf::SemWait (sem_t* sem)
{
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


bool Wolf::SemSignal (sem_t* sem)
{
   if (Conn::GetType() == "conn_fifo") {
      return true;
   }

   if (sem_post(sem) == -1) {
      perror("sem_post() ");
      return false;
   }

   return true;
}



void Wolf::HandleSignal (int sig, siginfo_t* info, void* ptr)
{
   static Wolf& wolf = GetInstance(0);
   switch (sig) {
      case SIGUSR1:
      { 
         if (wolf.is_client_connected) {
            std::cout << "Ignore handshake, client already connected" << std::endl; 
         } else {
            std::cout << "Client connected: pid is " << info->si_pid << std::endl; 
            wolf.is_client_connected = true;
  	    wolf.client_pid = info->si_pid;
         }
         break;       
      }
      case SIGUSR2:
      { 
         std::cout << "Client has error, disconnect him" << std::endl; 
         wolf.is_client_connected = false;
         wolf.client_pid = 0;
         break;       
      }
      case SIGTERM:
      {
         std::cout << "Terminate host" << std::endl; 
         if (wolf.is_client_connected) {
            wolf.is_client_connected = false;
            wolf.client_pid = 0;
         }
         wolf.Terminate();
         exit(EXIT_SUCCESS);
         break;
      }
      default:
      {
         std::cout << "Unknown signal: continue work" << std::endl; 
         break;
      }
   }
}

