#include <semaphore.h>
#include <signal.h>
#include <string.h>

#include "../connections/conn.h"


class Wolf {
public:
   ~Wolf ();

   static Wolf& GetInstance(int host_pid);
   
   void StartGame ();
   void PrepareGame ();
private:
   Wolf (int host_pid);
   
   Wolf (Wolf& w) = delete;
   Wolf& operator= (const Wolf& w) = delete;

   void Terminate ();
   
   int GetValue ();
   bool SemWait (sem_t* sem);   
   bool SemSignal (sem_t* sem);   
   
   static void HandleSignal (int sig, siginfo_t* info, void* ptr);
private:
   Conn conn;
   sem_t* sem_host;   
   sem_t* sem_client;   

   int goat_state;

   bool is_client_connected;
   int client_pid;
   int host_pid;

   std::string sem_host_name;
   std::string sem_client_name;
};
