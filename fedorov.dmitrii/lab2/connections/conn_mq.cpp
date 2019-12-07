#include <iostream>
#include <stdio.h>
#include <cstdlib>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <stdexcept>

#include "conn.h"

#include "../support/message_types.h"


Conn::Conn (int msgkey, bool create) {
   owner = create;
   desc = msgget(msgkey, IPC_CREAT | 0666);
   if (desc == -1) {
      throw std::runtime_error("msgget error");
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
   }
   
   std::cout << "Connection: msgqueue closed qid: " << desc << std::endl;
}

bool Conn::Read (void* buf, size_t count) {
   if (msgrcv(desc, buf, count - sizeof(long), 0, SA_RESTART) == -1) {
      perror("msgrcv() ");
      return false;
   } 
   return true;
}

bool Conn::Write (void* buf, size_t count) {
   if (msgsnd(desc, buf, count - sizeof(long), 0) == -1) {
      perror("msgsnd() ");
      return false;
   } 

   return true;
}

