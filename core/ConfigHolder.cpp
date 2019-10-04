//
// Created by dmitrii on 03.10.2019.
//

#include "ConfigHolder.h"
#include <fstream>
#include <sys/syslog.h>
#include <string>
#include <sstream>
#include <utility>
#include <vector>

ConfigHolder *ConfigHolder::m_inst = nullptr;

std::vector<std::string> ConfigHolder::get(const std::string& conf_elem_name){
    if (!m_inst)
        throw std::exception();
    auto it = m_config_params_map.find(conf_elem_name);
    if (it != m_config_params_map.cend())
        return it->second;
    else
        throw std::exception();
}

ConfigHolder::~ConfigHolder() {
    if (m_inst)
        m_config_params_map.clear();
}

void ConfigHolder::destroy() {
    delete m_inst;
}

ConfigHolder::ConfigHolder(const std::string &config_file_name) {
    std::fstream f(config_file_name);
    if (!f.is_open()){
        syslog(LOG_ERR, "can't open config file");
        throw std::exception();
    }

    std::string cur_line;
    while(getline(f, cur_line)){
        std::istringstream ss(cur_line);
        std::string key, value;
        std::getline(ss, key, ':');

        std::vector<std::string> values = std::vector<std::string>();
        while ( std::getline(ss, value, ','))
            values.push_back(value);

       m_config_params_map.insert(std::pair<std::string, std::vector<std::string>>(key, values));
    }

    f.close();
}

void ConfigHolder::init(const std::string& config_file_name){
    if (m_inst) {
        syslog(LOG_INFO, "ConfigHolder already initialized");
        return;
    }
    m_inst = new ConfigHolder(config_file_name);
}
ConfigHolder* ConfigHolder::getInstance(){
    if (m_inst)
        return m_inst;
    else
        syslog(LOG_ERR, "config holder is not init");
    throw std::exception();
}