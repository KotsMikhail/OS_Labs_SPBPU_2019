#pragma once
#include <semaphore.h>

class Conn;

class Goat {
public:
   static Goat& GetInstance (int host_pid);
   void PrepareGame ();
   void StartGame ();

private:
   Goat (int host_pid);
   void Terminate ();   

   int GenerateValue ();
   int IsAlive () { return status == 1; }   

   bool SemWait (sem_t* sem);   
   bool SemSignal (sem_t* sem);  
   
   static void OnSignalRecieve (int sig);

   Conn* conn;
   sem_t* sem_host;   
   sem_t* sem_client;   
   int host_pid;

   int status; //0 - alive, 1 - dead, 2 - seriously dead
};
