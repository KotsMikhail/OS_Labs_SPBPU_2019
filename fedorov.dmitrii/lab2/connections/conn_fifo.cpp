#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <fcntl.h>

#include "conn_fifo.h"

#include "../support/message_types.h"


std::string Conn::GetType ()
{
   return std::string("conn_fifo");
}


Conn::Conn (int host_pid_, bool create) {
   owner = create;
   host_pid = host_pid_;
   filename = std::string("/tmp/" + std::to_string(host_pid)).c_str();
   
   unlink(filename.c_str());
   
   int res = mkfifo(filename.c_str(), 0777);
   if (res == -1) {
      perror("mkfifo() ");
      exit(EXIT_FAILURE);
   }

   if (owner) {
      std::cout << "Connection: fifo created with name - " << filename << std::endl;
   } else {
      std::cout << "Connection: fifo opened with name - " << filename << std::endl;
   }
}


Conn::~Conn() {
   if (owner) {
      remove(filename.c_str());
      std::cout << "Connection: close fifo with name - " << filename << std::endl;
   }
}


bool Conn::Read (void* buf, size_t count) {
   Msg msg;
   int fifo_desc;
   
   if ((fifo_desc = open(filename.c_str(), O_RDONLY)) == -1) {
      perror("open() ");
      return false;
   } else {
      if (read(fifo_desc, &msg, count) == -1) {
         perror("read() ");
         close(fifo_desc);
      } else {
         *((Msg*)buf) = msg;
         close(fifo_desc);
         return true;
      }
   }
   return false;
}


bool Conn::Write (void* buf, size_t count) {
   int fifo_desc;
   
   if ((fifo_desc = open(filename.c_str(), O_WRONLY)) == -1) {
      perror("open() ");
      return false;
   } else {
      if (write(fifo_desc, buf, count) == -1) {
         perror("write() ");
         close(fifo_desc);
      } else {
         close(fifo_desc);
         return true;
      }
   }
   return false;
}

