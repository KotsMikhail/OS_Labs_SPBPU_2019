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
};
