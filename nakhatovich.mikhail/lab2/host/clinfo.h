#ifndef CLIENT_INFO_H
#define CLIENT_INFO_H

struct client_info_t
{
  int pid;
  bool is_attached;

  explicit client_info_t(int pid) : pid(pid), is_attached(pid != 0)
  {};
};

#endif // CLIENT_INFO_H
