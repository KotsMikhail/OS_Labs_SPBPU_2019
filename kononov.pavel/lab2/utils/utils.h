#ifndef WOLF_UTILS_H
#define WOLF_UTILS_H

#include <string>

#define SEMAPHORE_HOST_NAME "LAB2_HOST"
#define SEMAPHORE_CLIENT_NAME "LAB2_CLIENT"
static const int TIMEOUT = 5;

int GetRand(int limit);
std::string GetName(const char *const name, int id);

#endif //WOLF_UTILS_H
