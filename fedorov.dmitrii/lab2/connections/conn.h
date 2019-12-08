#pragma once

#include <cstdlib>
#include <string>


class Conn {
public:
   Conn (int id, bool create);
   ~Conn();

   bool Read (void* buf, size_t count);
   bool Write (void* buf, size_t count);

private:
   bool owner;
   int host_pid;
   int desc;
   
   int has_error;
   void* p_data;
};
