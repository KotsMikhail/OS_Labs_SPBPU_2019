//
// Created by dmitrii on 03.10.2019.
//

#ifndef INOTIFY_CONFIGHOLDER_H
#define INOTIFY_CONFIGHOLDER_H

#include <map>
#include <iostream>
#include <vector>

class ConfigHolder
{
public:
    std::vector<std::string> get(const std::string& conf_elem_name);
    static int init(const std::string& config_file_name);
    static ConfigHolder* getInstance();
    static void destroy();
    ConfigHolder(ConfigHolder &c) = delete;
    void operator=(ConfigHolder &c) = delete;
private:
    ~ConfigHolder();
    ConfigHolder();
    std::map<std::string, std::vector<std::string>> m_config_params_map;
    static ConfigHolder *m_inst;
    static char* m_config_file_name;
};


#endif //INOTIFY_CONFIGHOLDER_H
