#pragma once

#include <cstdlib>
#include <string>

class Conn
{
public:
   Conn (int id, bool create);
   ~Conn();

   bool Read (void* buf, size_t count);
   bool Write (void* buf, size_t count);

   static std::string GetType ();
private:
   bool owner;
   int host_pid;
   std::string socketpath;
   int hsocket; 
   int csocket;
};
