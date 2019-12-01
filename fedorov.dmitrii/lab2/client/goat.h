#pragma once
#include <semaphore.h>

class Conn;

class Goat {
public:
   ~Goat ();   


   static Goat* GetInstance (int host_pid);

   void PrepareGame ();
   void StartGame ();

private:
   Goat (int host_pid);   

   Goat (Goat& w) = delete;
   Goat& operator= (const Goat& w) = delete;  

   int GenerateValue ();
   bool GenAndWriteValue ();

   int IsAlive () { return status == 0; }   

   bool SemWait (sem_t* sem);   
   bool SemSignal (sem_t* sem);  
   
   static void OnSignalRecieve (int sig);

   Conn* conn;
   sem_t* sem_host;   
   sem_t* sem_client;   
   int host_pid;

   int status; //0 - alive, 1 - dead, 2 - seriously dead
};
