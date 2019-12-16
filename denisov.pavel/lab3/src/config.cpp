#include <new>
#include <iostream>
#include <algorithm>
#include <fstream>

#include "config.h"
#include "support.h"

CONFIG * CONFIG::instance = nullptr;
std::vector<std::string> CONFIG::keysNames = std::vector<std::string>({"writers_num_threads", "writers_num_records",
                                                                       "readers_num_threads", "readers_num_records",
                                                                       "general_n", "general_writers", "general_readers", "num_iterations"});


CONFIG::CONFIG (std::string &path)
    : filePath(path)
{
}


CONFIG * CONFIG::GetInstance (const char *path)
{
    if (path && !instance) {
        std::string strPath(path);
        std::string realPath = getRealPath(strPath);
        if (realPath.length() == 0 || isDir(realPath)) {
            std::cout << "Incorrect path to config file. Please try again..." << std::endl;
            return nullptr;
        }

        instance = new (std::nothrow) CONFIG(realPath);
    }

    return instance;
}


void CONFIG::Clear (void)
{
    delete instance;
    instance = nullptr;
}


bool CONFIG::Load (void)
{
    std::cout << filePath << std::endl;
    std::ifstream input(filePath);
    std::string line;

    if (input.is_open()) {
        while (std::getline(input, line)) {
            // Clear all space symbols
            line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());

            // check comment lines
            if ((line[0] == '/' && line[1] == '/') || line.empty()) {
                continue;
            }

            size_t separatorPos = line.find("=");
            if (separatorPos == std::string::npos) {
                std::cout << "Expected separator `=` in line \"" << line << "\"" << std::endl;
                input.close();
                return false;
            }

            std::string key = line.substr(0, separatorPos);
            std::string value = line.substr(separatorPos + 1);

            // "writers" or "readers"
            if (std::find(keysNames.begin(), keysNames.end(), key) != keysNames.end()) {
                int val;
                if (!fromStrToInt(value, val) || val == 0) {
                    std::cout << "Expected positive number after `=` for key " << key << "." << std::endl;
                    input.close();
                    return false;
                }
                values[key] = val;
            } else {
                std::cout << "Unexpected key " << key << std::endl;
                input.close();
                return false;
            }
        }

        input.close();
        return true;
    }

    return false;
}


int CONFIG::GetValue (CONFIG::KEYS key) const
{
    auto it = values.find(keysNames[(int)key]);
    if (it != values.end()) {
        return it->second;
    }

    return -1;
}
