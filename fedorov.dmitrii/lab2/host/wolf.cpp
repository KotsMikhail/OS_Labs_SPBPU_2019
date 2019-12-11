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
#include <stdexcept>

#include "../connections/conn.h"
#include "../support/message_types.h"
#include "../support/game_consts.h"

static const int g_timeout = 5;


Wolf::Wolf (int host_pid_)
   : conn(host_pid_, true) {
   sem_host_name = std::string("host_" + std::to_string(host_pid));
   sem_client_name = std::string("client_" + std::to_string(host_pid));

   is_client_connected = false;
   client_pid = 0;
   host_pid = host_pid_;
   
   sem_host = sem_open(sem_host_name.c_str(), O_CREAT, 0666, 0);
   if (sem_host == SEM_FAILED) {
      throw std::runtime_error("sem_host wasn't opened");
   }

   sem_client = sem_open(sem_client_name.c_str(), O_CREAT, 0666, 0);
   if (sem_client == SEM_FAILED) {
      sem_unlink(std::string("host_" + std::to_string(host_pid)).c_str());
      throw std::runtime_error("sem_client wasn't opened");
   }
   
   std::cout << "Wolf constructed" << std::endl; 
} 


Wolf::~Wolf () {
  if (is_client_connected) {
      kill(client_pid, SIGTERM);
   }

   if (sem_host != SEM_FAILED) {
      sem_unlink(sem_host_name.c_str());
   }
   
   if (sem_client != SEM_FAILED) {
      sem_unlink(sem_client_name.c_str());
   }

   std::cout << "Wolf terminated" << std::endl; 
}


Wolf& Wolf::GetInstance (int host_pid) {
   static Wolf wolf(host_pid);
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
   sigaction(SIGINT, &sa, nullptr);
   pause();
}


void Wolf::StartGame () {
   std::cout << "START GAME AS WOLF" << std::endl;
  
   while (true) {
      Msg msg;

      if (!SemWait(sem_host)) {
         return;
      }      

      std::cout << "___________GAME_STEP___________" << std::endl;

      if (!conn.Read(&msg, sizeof(msg))) {
         return;
      }

      int goat_val = msg.data;
      std::cout << "Goat num: " << goat_val << std::endl;   
     
      int cur_val = GetValue();
      std::cout << "Wolf: num is " << cur_val << std::endl;
      
      if (goat_state == 0 && abs(goat_val - cur_val) > g_alive_deviation) {
         goat_state++;
      } else if (goat_state != 0) {
         if (goat_state == 1 && abs(goat_val - cur_val) > g_dead_deviation) {
            goat_state++;
         } else {
            goat_state--;
         }
      }

      if (goat_state == 0) {
         std::cout << "Goat: I am alive with full hp" << std::endl;
      } else if (goat_state == 1) {
         std::cout << "Goat: I am dead, but there is a chance to be revived" << std::endl;
      } else if (goat_state == 2) {
         std::cout << "Goat: I am dead" << std::endl;
      }   

      msg.type = 1;
      msg.data = goat_state;
      if (!conn.Write(&msg, sizeof(msg))) {
         return;
      }

      if (goat_state == 2) {
         std::cout << "Kill client" << std::endl;
         kill(client_pid, SIGUSR1);
         pause();
         return;
      }

      if (!SemSignal(sem_client)) {
         return;
      }
   }
}


int Wolf::GetValue () {
   int res;
   std::cout << "Enter number from " << g_min_wolf_val << " to " << g_max_wolf_val << std::endl;

   do {
      std::cin >> res;
      if (std::cin.fail()) {
         std::cin.clear();
      }
   } while (res < g_min_wolf_val || res > g_max_wolf_val);
   return res;
}


bool Wolf::SemWait (sem_t* sem) {
   struct timespec ts;
   clock_gettime(CLOCK_REALTIME, &ts);
   ts.tv_sec += g_timeout;
   if (sem_timedwait(sem, &ts) == -1) {
      perror("sem_timewait() ");
      return false;
   }

   return true;
}


bool Wolf::SemSignal (sem_t* sem) {
   if (sem_post(sem) == -1) {
      perror("sem_post() ");
      return false;
   }
   return true;
}



void Wolf::HandleSignal (int sig, siginfo_t* info, void* ptr) {
   static Wolf& wolf = GetInstance(0);
   switch (sig) {
      case SIGUSR1:
         if (wolf.is_client_connected) {
            std::cout << "Ignore handshake, client already connected" << std::endl; 
            kill(info->si_pid, SIGTERM);
         } else {
            std::cout << "Client connected: pid is " << info->si_pid << std::endl; 
            wolf.is_client_connected = true;
            wolf.client_pid = info->si_pid;
         }
         break;       
      case SIGUSR2: 
         std::cout << "Client has error, disconnect him" << std::endl; 
         wolf.is_client_connected = false;
         wolf.client_pid = 0;
         break;       
      case SIGTERM:
      case SIGINT:
         std::cout << "Terminate host" << std::endl; 
         exit(EXIT_SUCCESS);
         break;
      default:
         std::cout << "Unknown signal: continue work" << std::endl; 
         break;
   }
}

