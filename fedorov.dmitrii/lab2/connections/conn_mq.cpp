#include <iostream>
#include <stdio.h>
#include <cstdlib>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

#include "conn_mq.h"

#include "../support/message_types.h"


std::string Conn::GetType ()
{
   return std::string("conn_mq");
}


Conn::Conn (int msgkey, bool create) {
   owner = create;
   qid = msgget(msgkey, IPC_CREAT | 0666);
   if (qid == -1) {
      perror("msgget() ");
      exit(EXIT_FAILURE);
   }
   if (owner) {
      std::cout << "Connection: msg queue created with qid " << qid << std::endl;
   } else {
      std::cout << "Connection: msg queue opened with qid " << qid << std::endl;
   }
}

Conn::~Conn() {
   if (!owner) {
      return;
   }
  
   if (msgctl(qid, IPC_RMID, 0) == -1) {
      perror("msqctl() ");
   }
   
   std::cout << "Connection: msgqueue closed qid: " << qid << std::endl;
}

bool Conn::Read (void* buf, size_t count) {
   if (msgrcv(qid, buf, count - sizeof(long), 0, SA_RESTART) == -1) {
      perror("msgrcv() ");
      return false;
   } 
   return true;
}

bool Conn::Write (void* buf, size_t count) {
   if (msgsnd(qid, buf, count - sizeof(long), 0) == -1) {
      perror("msgsnd() ");
      return false;
   } 

   return true;
}

