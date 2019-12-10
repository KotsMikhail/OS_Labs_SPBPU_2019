#include <iostream>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdexcept>

#include "conn.h"

#include "../support/message_types.h"


struct SockConnData {
   int hsocket;
   int csocket;
};


Conn::Conn (int host_pid_, bool create) { 
   owner = create;
   host_pid = host_pid_;
   std::string socketpath = std::string("/tmp/" + std::to_string(host_pid));
   SockConnData socketdata;

   struct sockaddr_un serv_addr;
   serv_addr.sun_family = AF_UNIX;
   strcpy(serv_addr.sun_path, socketpath.c_str());
   std::cout << "Socket path: " << socketpath << std::endl;

   if (create) {
      std::cout << "Create listener" << std::endl;
      socketdata.hsocket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
      if (socketdata.hsocket == -1) {
         throw std::runtime_error("hsocket error");
      }
      
      std::cout << "Bind listener" << std::endl;
      if (bind(socketdata.hsocket, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
         throw std::runtime_error("bind error");
      }

      std::cout << "Listen" << std::endl;
      if (listen(socketdata.hsocket, 1) < 0 ) {
         throw std::runtime_error("listen error");
      }
 
      std::cout << "Accept" << std::endl;
      socketdata.csocket = accept(socketdata.hsocket, NULL, NULL);
      if (socketdata.csocket < 0) {
         throw std::runtime_error("accept error");
      }
   } else {
      socketdata.csocket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
      if (socketdata.csocket < 0) { 
         throw std::runtime_error("csocket error");
      }
      std::cout << "Socket created" << std::endl;
  
      if (connect(socketdata.csocket,(struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
         throw std::runtime_error("connect error");
      }   
      std::cout << "Socket connected" << std::endl;
   }
   
   p_data = new (std::nothrow) SockConnData(socketdata); 
}


Conn::~Conn () {
   SockConnData* socketdata = (SockConnData*)p_data;
   std::string socketpath = std::string("/tmp/" + std::to_string(host_pid));
   
   if (!owner) {
      if (socketdata->csocket != -1) {
         if (close(socketdata->csocket) < 0) {
            perror("close(csocket) ");
         }
      }
   }

   if (owner) {
      if (socketdata->hsocket != -1) {
         if (close(socketdata->hsocket) < 0) {
            perror("close(hsocket) ");
         }
      } 

      unlink(socketpath.c_str());
   }

   delete (SockConnData*)p_data;
   std::cout << "Connection closed" << std::endl;  
}


bool Conn::Read (void* buf, size_t count) {
    SockConnData* socketdata = (SockConnData*)p_data;
    int n = recv(socketdata->csocket, buf, count, 0);
    if (n < 0) { 
       perror("read()");
       return false;
    }
    return true;
}


bool Conn::Write (void* buf, size_t count) {
   SockConnData* socketdata = (SockConnData*)p_data;
   int n = send(socketdata->csocket, buf, count, MSG_NOSIGNAL);
   if (n < 0) { 
      perror("write() ");
      return false;
   }
   return true;
}

