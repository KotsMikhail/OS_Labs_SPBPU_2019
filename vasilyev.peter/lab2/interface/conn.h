//
// Created by peter on 11/22/19.
//

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

  static void throwError( const std::string &message )
  {
    std::ostringstream ss;
    ss << message << ", errno = " << strerror(errno);
    throw std::runtime_error(ss.str());
  } // end of 'throwError' function
  
public:
  void open( const std::string &id, bool is_create );
  void close();
  int read();
  void write( int message );
};

#endif //LAB2_CONN_H
