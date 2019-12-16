#ifndef TEST_H
#define TEST_H

#include <set.h>
#include <string>
#include <exception>
#include <utility>
#include <vector>

struct TestException: public std::exception {
private:
  std::string message;
public:
  explicit TestException(std::string& message) noexcept: message(message)
  {}

  const char* what() const noexcept override
  {
    return message.c_str();
  }
};

template<typename T>
using data_set = std::vector<T>;

template<typename T>
struct TestInfo
{
  Set<T>* testing_set;
  data_set<T> data;

  TestInfo(Set<T>* set, data_set<T> data) noexcept: testing_set(set), data(std::move(data))
  {}
};

enum class TestType
{
  WRITE,
  READ,
  GENERAL
};

template<typename T>
class Test
{
public:
  std::string name;

  explicit Test(std::string name) noexcept: name(std::move(name))
  {}
  virtual void run() = 0;
  virtual void check() const noexcept(false) = 0;
  virtual ~Test() = default;
};

#endif // TEST_H
