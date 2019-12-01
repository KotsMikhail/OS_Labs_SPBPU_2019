#include <iostream>
#include <stdio.h>
#include <cstdlib>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

#include "conn.h"

#include "../support/message_types.h"


std::string Conn::GetType ()
{
   return std::string("conn_mq");
}

bool Conn::HasError ()
{
   return has_error;
}

Conn::Conn (int msgkey, bool create) {
   owner = create;
   desc = msgget(msgkey, IPC_CREAT | 0666);
   if (desc == -1) {
      perror("msgget() ");
      RETURN_WITH_ERROR;
   }
   if (owner) {
      std::cout << "Connection: msg queue created with qid " << desc << std::endl;
   } else {
      std::cout << "Connection: msg queue opened with qid " << desc << std::endl;
   }
}

Conn::~Conn() {
   if (!owner) {
      return;
   }
  
   if (msgctl(desc, IPC_RMID, 0) == -1) {
      perror("msqctl() ");
      RETURN_WITH_ERROR;
   }
   
   std::cout << "Connection: msgqueue closed qid: " << desc << std::endl;
}

bool Conn::Read (void* buf, size_t count) {
   if (msgrcv(desc, buf, count - sizeof(long), 0, SA_RESTART) == -1) {
      perror("msgrcv() ");
      RETURN_FALSE_WITH_ERROR;
   } 
   return true;
}

bool Conn::Write (void* buf, size_t count) {
   if (msgsnd(desc, buf, count - sizeof(long), 0) == -1) {
      perror("msgsnd() ");
      RETURN_FALSE_WITH_ERROR;
   } 

   return true;
}

