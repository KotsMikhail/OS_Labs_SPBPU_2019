//
// Created by dmitrii on 03.10.2019.
//

#ifndef INOTIFY_CONFIGHOLDER_H
#define INOTIFY_CONFIGHOLDER_H

#include <map>
#include <iostream>
#include <vector>

class ConfigHolder {
public:
    std::vector<std::string> get(const std::string& conf_elem_name);
    static void init(const std::string& config_file_name);
    static ConfigHolder* getInstance();
    static void destroy();
private:
    ~ConfigHolder();
    ConfigHolder(const std::string& config_file_name);
    std::map<std::string, std::vector<std::string>> m_config_params_map;
    static ConfigHolder *m_inst;
};


#endif //INOTIFY_CONFIGHOLDER_H
