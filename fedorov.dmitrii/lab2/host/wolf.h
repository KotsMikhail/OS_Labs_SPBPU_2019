#include <semaphore.h>
#include <signal.h>


class Conn;

class Wolf {
public:
   static Wolf& GetInstance(int host_pid);
   void StartGame ();
   void PrepareGame ();
private:
   Wolf (int host_pid);
   void Terminate ();   

   int GenerateValue ();
   bool SemWait (sem_t* sem);   
   bool SemSignal (sem_t* sem);   
   
   static void HandleSignal (int sig, siginfo_t* info, void* ptr);
private:
   Conn* conn;
   sem_t* sem_host;   
   sem_t* sem_client;   

   int goat_state;

   bool is_client_connected;
   int client_pid;
};
