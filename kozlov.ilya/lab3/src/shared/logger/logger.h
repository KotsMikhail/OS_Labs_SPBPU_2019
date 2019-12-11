#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <iostream>

class Logger
{
private:
  static pthread_mutex_t mutex;
public:
  static void init();
  static void logError(const std::string& tag, const std::string& msg);
  static void logDebug(const std::string& tag, const std::string& msg);
};

#endif // LOGGER_H
