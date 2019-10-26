#ifndef CONFIG_H
#define CONFIG_H

#include <string>

class Config
{
public:
    static Config& GetInstance(char *cfgName = nullptr);
    const std::string& GetAbsoluteFolderPath();
    int GetInterval();
    void ReadConfig();

private:
    std::string absPath, absfldrPath;
    int interval;

    Config(char *cfgName);
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
};

#endif //CONFIG_H
