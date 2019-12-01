#pragma once

#include <cstdlib>
#include <string>

#define RETURN_WITH_ERROR has_error = true; return;
#define RETURN_FALSE_WITH_ERROR has_error = true; return false;


class Conn
{
public:
   Conn (int id, bool create);
   ~Conn();

   bool Read (void* buf, size_t count);
   bool Write (void* buf, size_t count);
   
   bool HasError ();

   static std::string GetType ();
private:
   bool owner;
   int host_pid;
   int desc;
   
   int has_error;
   int* internal_data;
};
