#include <iostream>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>

#include "conn.h"

#include "../support/message_types.h"


std::string Conn::GetType ()
{
   return std::string("conn_sock");
}

bool Conn::HasError ()
{
   return has_error;
}

Conn::Conn (int host_pid_, bool create) { 
   owner = create;
   host_pid = host_pid_;
   std::string socketpath = std::string("/tmp/" + std::to_string(host_pid));
   internal_data = new (std::nothrow) int[2]; 

   struct sockaddr_un serv_addr;
   serv_addr.sun_family = AF_UNIX;
   strcpy(serv_addr.sun_path, socketpath.c_str());
   std::cout << "Socket path: " << socketpath << std::endl;

   if (create) {
      std::cout << "Create listener" << std::endl;
      internal_data[0] = socket(AF_UNIX, SOCK_SEQPACKET, 0);
      if (internal_data[0] == -1) {
         perror("socket() ");
         RETURN_WITH_ERROR;
      }
      
      std::cout << "Bind listener" << std::endl;
      if (bind(internal_data[0], (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
         perror("bind() ");
         RETURN_WITH_ERROR;
      }

      std::cout << "Listen" << std::endl;
      if (listen(internal_data[0], 1) < 0 ) {
         perror("listen() ");
         RETURN_WITH_ERROR
      }
 
      std::cout << "Accept" << std::endl;
      internal_data[1] = accept(internal_data[0], NULL, NULL);
      if (internal_data[1] < 0) {
         perror("accept() ");
         RETURN_WITH_ERROR;
      }
   } else {
      internal_data[1] = socket(AF_UNIX, SOCK_SEQPACKET, 0);
      if (internal_data[1] < 0) { 
         perror("socket() ");
         RETURN_WITH_ERROR;
      }
      std::cout << "Socket created" << std::endl;
  
      if (connect(internal_data[1],(struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
         perror("connect()");
         RETURN_WITH_ERROR;
      }   
      std::cout << "Socket connected" << std::endl;
   }
}

Conn::~Conn() {
   std::string socketpath = std::string("/tmp/" + std::to_string(host_pid));
   if (!owner) {
      if (close(internal_data[1]) < 0) {
         perror("close(csocket) ");
      }
   }

   if (owner) {
      if (internal_data[0] != -1) {
         if (close(internal_data[0]) < 0) {
            perror("close(hsocket) ");
         }
      } 

      unlink(socketpath.c_str());
   }
   std::cout << "Connection closed" << std::endl;
}

bool Conn::Read (void* buf, size_t count) {
    int n = recv(internal_data[1], buf, count, 0);
    if (n < 0) { 
       perror("read()");
       RETURN_FALSE_WITH_ERROR;
    }
    return true;
}

bool Conn::Write (void* buf, size_t count) {
    int n = send(internal_data[1], buf, count, MSG_NOSIGNAL);
    if (n < 0) { 
       perror("write() ");
       RETURN_FALSE_WITH_ERROR;
    }
   return true;
}

