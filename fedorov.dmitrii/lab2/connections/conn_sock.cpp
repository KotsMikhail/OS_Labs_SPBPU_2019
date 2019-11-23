#include <iostream>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>

#include "conn_sock.h"

#include "../support/message_types.h"


std::string Conn::GetType ()
{
   return std::string("conn_sock");
}

Conn::Conn (int host_pid_, bool create) { 
   owner = create;
   host_pid = host_pid_;
   socketpath = std::string("/tmp/" + std::to_string(host_pid));
 
   struct sockaddr_un serv_addr;
   serv_addr.sun_family = AF_UNIX;
   strcpy(serv_addr.sun_path, socketpath.c_str());
   std::cout << "Socket path: " << socketpath << std::endl;

   if (create) {
      std::cout << "Create listener" << std::endl;
      hsocket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
      if (hsocket == -1) {
         perror("socket() ");
         exit(EXIT_FAILURE);
      }
      
      std::cout << "Bind listener" << std::endl;
      if (bind(hsocket, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
         perror("bind() ");
         close(hsocket);
         exit(EXIT_FAILURE);
      }

      std::cout << "Listen" << std::endl;
      if (listen(hsocket, 1) < 0 ) {
         perror("listen() ");
         close(hsocket);
         unlink(socketpath.c_str());
         exit(EXIT_FAILURE);
      }
 
      std::cout << "Accept" << std::endl;
      csocket = accept(hsocket, NULL, NULL);
      if (csocket < 0) {
         perror("accept() ");
         close(hsocket);
         unlink(socketpath.c_str());
         exit(EXIT_FAILURE);
      }
   } else {
      csocket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
      if (csocket < 0) { 
         perror("socket() ");
         exit(EXIT_FAILURE);
      }
      std::cout << "Socket created" << std::endl;
  
      if (connect(csocket,(struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
         perror("connect()");
         close(csocket);
         exit(EXIT_FAILURE);
      }   
      std::cout << "Socket connected" << std::endl;
   }
}

Conn::~Conn() {
   if (!owner) {
      if (close(csocket) < 0) {
         perror("close(csocket) ");
         exit(EXIT_FAILURE);
      }
   }
   if (owner) {
      if (close(csocket) < 0) {
         perror("close(csocket) ");
         exit(EXIT_FAILURE);
      }
      if (close(hsocket) < 0) {
         perror("close(hsocket) ");
         exit(EXIT_FAILURE);
      } 
      unlink(socketpath.c_str());
   }
   std::cout << "Connection closed" << std::endl;
}

bool Conn::Read (void* buf, size_t count) {
    int n = recv(csocket, buf, count, 0);
    if (n < 0) { 
       perror("read()");
       return false; 
    }
    return true;
}

bool Conn::Write (void* buf, size_t count) {
    int n = send(csocket, buf, count, MSG_NOSIGNAL);
    if (n < 0) { 
       perror("write() ");
       return false; 
    }
   return true;
}

