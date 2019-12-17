#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

#include <algorithm>

#include "config.h"
#include "utils.h"

config_t * config_t::_instance = nullptr;
vector_string_t config_t::_keys = vector_string_t({"writers_th", "writers_num", "readers_th", "readers_num",
                                                   "common_n", "common_writers", "common_readers", "time_iterations"});

config_t::config_t(string_t &path) : _conf_path(path)
{}

config_t * config_t::get_instance(const char *path)
{
    if (!_instance && path)
    {
        string_t conf_path(path);
        conf_path = get_realpath(conf_path);
        if (conf_path.length() == 0 || is_dir(conf_path))
        {
            printf("Incorrect path to configuration file.\n");
            return nullptr;
        }
        _instance = new (std::nothrow) config_t(conf_path);
    }
    return _instance;
}

void config_t::destroy()
{
    delete _instance;
}

bool config_t::load()
{
    ifstream_t config(_conf_path);
    string_t line, part_line;
    size_t n;
    bool ret = true;
    if (config.is_open())
    {
        while (std::getline(config, line))
        {
            line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
            if (line[0] == '#' || line.empty())
                continue;
            size_t del_pos = line.find("=");
            if (del_pos == string_t::npos)
            {
                printf("Expected one `=` in line `%s`.\n", line.c_str());
                ret = false;
                break;
            }
            string_t name = line.substr(0, del_pos), value = line.substr(del_pos + 1);
            if (value.find("=") != string_t::npos)
            {
                printf("Expected only one `=` in line `%s`.\n", line.c_str());
                ret = false;
                break;
            }
            if (std::find(_keys.begin(), _keys.end(), name) != _keys.end())
            {
                if (!to_size_t(value, n) || n == 0)
                {
                    printf("Expected positive integer number after `=` in line `%s`.\n", line.c_str());
                    ret = false;
                    break;
                }
                _values[name] = n;
            }
            else
                printf("Unexpected word `%s` before `=` in `%s`.\n", name.c_str(), line.c_str());
        }
        config.close();
        return ret;
    }
    return false;
}

size_t config_t::get_value(size_types_t type) const
{
    auto it = _values.find(_keys[(size_t)type]);
    if (it != _values.cend())
        return it->second;
    return 0;
}
