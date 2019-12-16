#ifndef CONFIG_H
#define CONFIG_H

#include "types.h"

enum size_types_t
{
    WRITERS_TH = 0,
    WRITERS_NUM,
    READERS_TH,
    READERS_NUM,
    COMMON_N,
    COMMON_WRITERS,
    COMMON_READERS,
    TIME_ITERATIONS
};

class config_t
{
public:
    static config_t * get_instance(const char *path=nullptr);
    static void destroy();
    
    bool load();

    size_t get_value(size_types_t type) const;

private: 
    config_t() = delete;
    config_t(config_t const&) = delete;
    config_t& operator=(config_t const&) = delete;

    config_t(string_t &path);
    ~config_t() = default;

    static config_t * _instance;
    static vector_string_t _keys, _array_keys;

    string_t _conf_path;
    vector_size_t _cnt_rec, _cnt_read;
    map_string_size_t _values;
};

#endif // CONFIG_H
