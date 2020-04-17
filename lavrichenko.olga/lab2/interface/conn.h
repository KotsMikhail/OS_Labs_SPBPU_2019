#ifndef LAB2_CONN_H
#define LAB2_CONN_H

#include <map>
#include <string>
#include <sstream>
#include <cstring>

class Connection
{
private:
  int m_desc[2];
  bool m_is_owner;
  std::string m_id;

public:
  void open( const std::string &id, bool is_create );
  void close();
  int read();
  void write( int message );
};

#endif //LAB2_CONN_H
