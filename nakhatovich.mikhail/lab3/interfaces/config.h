#ifndef CONFIG_H
#define CONFIG_H

#include "types.h"

enum size_types_t
{
    WRITERS = 0,
    READERS,
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

    const size_t get_value(size_types_t type) const;
    const vector_size_t & get_cnt_elements(size_types_t type) const;

private: 
    config_t() = delete;
    config_t(config_t const&) = delete;
    config_t& operator=(config_t const&) = delete;

    config_t(string_t &path);
    ~config_t();

    bool parse_with_array(string_t &s, string_t &type, string_t &line);
    bool parse_without_array(string_t &s, string_t &type, string_t &line);

    static config_t * _instance;
    static vector_string_t _keys, _array_keys;

    string_t _conf_path;
    vector_size_t _cnt_rec, _cnt_read;
    map_string_size_t _values;
};

#endif // CONFIG_H
