#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

#include <algorithm>

#include "config.h"
#include "utils.h"

config_t * config_t::_instance = nullptr;
vector_string_t config_t::_keys = vector_string_t({"writers", "readers", "common_n", "common_writers", "common_readers", "time_iterations"});
vector_string_t config_t::_array_keys = vector_string_t({"writers", "readers"});

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
            if (std::find(_array_keys.begin(), _array_keys.end(), name) != _array_keys.end())
            {
                if (!parse_with_array(value, name, line))
                {
                    ret = false;
                    break;
                }
            }
            else if (std::find(_keys.begin(), _keys.end(), name) != _keys.end())
            {
                if (!parse_without_array(value, name, line))
                {
                    ret = false;
                    break;
                }
            }
            else
                printf("Unexpected word `%s` before `=` in `%s`.\n", name.c_str(), line.c_str());
        }
        config.close();
        return ret;
    }
    return false;
}

bool config_t::parse_with_array(string_t &s, string_t &type, string_t &line)
{
    size_t del_pos = s.find("|");
    if (del_pos == string_t::npos)
    {
        printf("Expected one `|` after `=` in line `%s`.\n", line.c_str());
        return false;
    }
    string_t sn = s.substr(0, del_pos), sval = s.substr(del_pos + 1);
    if (sval.find("=") != string_t::npos)
    {
        printf("Expected only one `|` after `=` in line `%s`.\n", line.c_str());
        return false;
    }
    size_t n, tmp;
    if ((!to_size_t(sn, n) || n == 0))
    {
        printf("Expected positive integer number after `=` in line `%s`.\n", line.c_str());
        return false;
    }
    vector_size_t vs;
    iss_t iss(sval);
    string_t part_line;
    while (std::getline(iss, part_line, ','))
    {
        if (!to_size_t(part_line, tmp))
        {
            printf("Expected nonnegative integer number after `|` in line `%s`.\n", line.c_str());
            return false;
        }
        vs.push_back(tmp);
    }
    if (vs.size() != n)
    {
        printf("Expected %lu nonnegative integer numbers after `|` in line `%s`.\n", n, line.c_str());
        return false;
    }
    if (type == "writers")
        _cnt_rec = vs;
    else
        _cnt_read = vs;
    _values[type] = n;
    return true;
}

bool config_t::parse_without_array(string_t &s, string_t &type, string_t &line)
{
    size_t n;
    if ((!to_size_t(s, n) || n == 0))
    {
        printf("Expected positive integer number after `=` in line `%s`.\n", line.c_str());
        return false;
    }
    _values[type] = n;
    return true;
}

size_t config_t::get_value(size_types_t type) const
{
    auto it = _values.find(_keys[type]);
    if (it != _values.cend())
        return it->second;
    return 0;
}

const vector_size_t & config_t::get_cnt_elements(size_types_t type) const
{
    if (type == WRITERS)
        return _cnt_rec;
    return _cnt_read;
}
