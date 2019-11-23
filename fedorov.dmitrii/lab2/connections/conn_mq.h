#pragma once

#include <sys/types.h>
#include <cstdio>


class Conn {
public:
   Conn (int id, bool create);
   ~Conn ();

   bool Read (void* buf, size_t count);
   bool Write (void* buf, size_t count);

   static std::string GetType ();
private:
   int qid;
   bool owner;
};
