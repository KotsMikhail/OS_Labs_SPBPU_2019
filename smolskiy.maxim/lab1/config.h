#ifndef CONFIG_H
#define CONFIG_H

#include <string>

class Config
{
public:
    std::string absPath, absfldrPath;
    int interval;

    static Config& GetInstance(char *cfgName = nullptr);
    void ReadConfig();

private:
    Config(char *cfgName);
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
};

#endif //CONFIG_H
