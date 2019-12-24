#include "logger.h"

pthread_mutex_t Logger::mutex;
Util::Modifier Logger::def = Util::Modifier(Util::Color::DEFAULT);
Util::Modifier Logger::red = Util::Modifier(Util::Color::RED);

bool Logger::init()
{
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  return !pthread_mutex_init(&mutex, &attr);
}

void Logger::destroy()
{
  pthread_mutex_destroy(&mutex);
}

void Logger::logError(const std::string& tag, const std::string& msg)
{
  pthread_mutex_lock(&mutex);

  std::cout << red << "ERROR: " << tag << "::" << msg << def << std::endl;
  pthread_mutex_unlock(&mutex);
}

void Logger::logDebug(const std::string& tag, const std::string& msg)
{
#ifdef LOG_DEBUG
  pthread_mutex_lock(&mutex);
  std::cout << "DEBUG: " << tag << "::" << msg << std::endl;
  pthread_mutex_unlock(&mutex);
#endif
}
