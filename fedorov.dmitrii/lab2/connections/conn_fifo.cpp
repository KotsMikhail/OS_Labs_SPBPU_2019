#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <fcntl.h>
#include <unistd.h>

#include "conn.h"

#include "../support/message_types.h"


std::string Conn::GetType ()
{
   return std::string("conn_fifo");
}

bool Conn::HasError ()
{
   return has_error;
}

Conn::Conn (int host_pid_, bool create) {
   owner = create;
   host_pid = host_pid_;
   std::string filename = std::string("/tmp/" + std::to_string(host_pid)).c_str();
   
   
   if (owner) {
      unlink(filename.c_str());
      int res = mkfifo(filename.c_str(), 0777);
      if (res == -1) {
         perror("mkfifo() ");
         RETURN_WITH_ERROR;
      }
   }

   if ((desc = open(filename.c_str(), O_RDWR)) == -1) {
      perror("open() ");
      RETURN_WITH_ERROR;
   }

   if (owner) {
      std::cout << "Connection: fifo created with name - " << filename << std::endl;
   } else {
      std::cout << "Connection: fifo opened with name - " << filename << std::endl;
   }
}


Conn::~Conn() {
   std::string filename = std::string("/tmp/" + std::to_string(host_pid)).c_str();

   if (owner) {
      unlink(filename.c_str());
      std::cout << "Connection: close fifo with name - " << filename << std::endl;
   }
}


bool Conn::Read (void* buf, size_t count) {
   Msg msg;

   std::cout << "bef read"  << std::endl;
 
   if (read(desc, &msg, count) == -1) {
      perror("read() ");
      RETURN_FALSE_WITH_ERROR;
   } 

   std::cout << "aft read"  << std::endl;
 
     
   *((Msg*)buf) = msg;
   return true;   
}


bool Conn::Write (void* buf, size_t count) {
   if (write(desc, buf, count) == -1) {
      perror("write() ");
      RETURN_FALSE_WITH_ERROR;
   } 

   return true;
}

