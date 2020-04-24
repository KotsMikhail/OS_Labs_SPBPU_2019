#include "utils.h"
#include <random>

extern int GetRand(int limit) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(1, limit);
    return dist(mt);
}


extern std::string GetName(const char *const name, int id) {
    std::string res(name);
    return res + std::to_string(id);
}
