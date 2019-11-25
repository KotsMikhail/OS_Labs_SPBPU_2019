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
         exit(EXIT_FAILURE);
      }
      
      std::cout << "Bind listener" << std::endl;
      if (bind(internal_data[0], (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
         perror("bind() ");
         close(internal_data[0]);
         exit(EXIT_FAILURE);
      }

      std::cout << "Listen" << std::endl;
      if (listen(internal_data[0], 1) < 0 ) {
         perror("listen() ");
         close(internal_data[0]);
         unlink(socketpath.c_str());
         exit(EXIT_FAILURE);
      }
 
      std::cout << "Accept" << std::endl;
      internal_data[1] = accept(internal_data[0], NULL, NULL);
      if (internal_data[1] < 0) {
         perror("accept() ");
         close(internal_data[0]);
         unlink(socketpath.c_str());
         exit(EXIT_FAILURE);
      }
   } else {
      internal_data[1] = socket(AF_UNIX, SOCK_SEQPACKET, 0);
      if (internal_data[1] < 0) { 
         perror("socket() ");
         exit(EXIT_FAILURE);
      }
      std::cout << "Socket created" << std::endl;
  
      if (connect(internal_data[1],(struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
         perror("connect()");
         close(internal_data[1]);
         exit(EXIT_FAILURE);
      }   
      std::cout << "Socket connected" << std::endl;
   }
}

Conn::~Conn() {
   std::string socketpath = std::string("/tmp/" + std::to_string(host_pid));

   if (!owner) {
      if (close(internal_data[1]) < 0) {
         perror("close(csocket) ");
         exit(EXIT_FAILURE);
      }
   }
   if (owner) {
      if (close(internal_data[1]) < 0) {
         perror("close(csocket) ");
         exit(EXIT_FAILURE);
      }
      if (close(internal_data[0]) < 0) {
         perror("close(hsocket) ");
         exit(EXIT_FAILURE);
      } 
      unlink(socketpath.c_str());
   }
   std::cout << "Connection closed" << std::endl;
}

bool Conn::Read (void* buf, size_t count) {
    int n = recv(internal_data[1], buf, count, 0);
    if (n < 0) { 
       perror("read()");
       return false; 
    }
    return true;
}

bool Conn::Write (void* buf, size_t count) {
    int n = send(internal_data[1], buf, count, MSG_NOSIGNAL);
    if (n < 0) { 
       perror("write() ");
       return false; 
    }
   return true;
}

