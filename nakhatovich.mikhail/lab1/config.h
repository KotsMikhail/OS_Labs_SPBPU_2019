#ifndef DISK_MONITOR_CONFIG_H
#define DISK_MONITOR_CONFIG_H

#include "types.h"

class config_t
{
public:
    ~config_t();

    static config_t * init(const char *path);
    void load();
    const set_string_t & get_difference_delete() const;
    const set_string_t & get_difference_add() const;

private: 
    config_t() = delete;
    config_t(string_t &path);

    string_t _conf_path;
    set_string_t _directories;
    set_string_t _deleted_directories, _added_directories;

    set_string_t load_directories();
};

#endif // DISK_MONITOR_CONFIG_H