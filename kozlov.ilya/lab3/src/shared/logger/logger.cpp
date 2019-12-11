#include "logger.h"

pthread_mutex_t Logger::mutex;

void Logger::init()
{
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutex_init(&mutex, &attr);
}

void Logger::logError(const std::string& tag, const std::string& msg)
{
  pthread_mutex_lock(&mutex);
  std::cout << "ERROR:" << tag << "::" << msg << std::endl;
  pthread_mutex_unlock(&mutex);
}

void Logger::logDebug(const std::string& tag, const std::string& msg)
{
#ifdef LOG_DEBUG
  pthread_mutex_lock(&mutex);
  std::cout << tag << "::" << msg << std::endl;
  pthread_mutex_unlock(&mutex);
#endif
}
