#ifndef TYPES_H
#define TYPES_H

#include <pthread.h>

#include <fstream>
#include <map>
#include <string>
#include <sstream>
#include <vector>

typedef std::string                   string_t;
typedef std::vector<int>              vector_int_t;
typedef std::vector<vector_int_t>     vvector_int_t;
typedef std::vector<size_t>           vector_size_t;
typedef std::vector<string_t>         vector_string_t;
typedef std::istringstream            iss_t;
typedef std::stringstream             ss_t;
typedef std::ifstream                 ifstream_t;
typedef std::vector<pthread_t>        vector_pthread_t;
typedef std::pair<string_t, size_t>   pair_string_size_t;
typedef std::map<string_t, size_t>    map_string_size_t;

#endif // TYPES_H
