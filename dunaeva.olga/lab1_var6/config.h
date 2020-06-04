#ifndef CONFIG_H
#define CONFIG_H

#include <string>

class Config
{
public:
    void ReadConfig();
    int GetInterval();
    const std::string& GetFolderPath();
    static Config& GetInstance(char *confName = nullptr);


private:
    std::string configPath;
    std::string folderPath;
    int interval;

    Config(char *confName);
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    void CheckConfigValues();
};

#endif //CONFIG_H


