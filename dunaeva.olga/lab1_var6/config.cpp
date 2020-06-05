#include <syslog.h>
#include <fstream>
#include <limits.h>
#include <dirent.h>
#include "config.h"


using namespace std;

void Config::CheckConfigValues()
{
    if (!folderPath.length() || interval < 1)
    {
        syslog(LOG_ERR, "Error: Wrong values in config file.");
        exit(EXIT_FAILURE);
    }

    DIR *dir = opendir(folderPath.c_str());

    if (dir == nullptr)
    {
        syslog(LOG_ERR, "Error: Path isn't existing.");
        exit(EXIT_FAILURE);
    }
}

void Config::ReadConfig()
{
    ifstream configFile(configPath);

    if (configFile.is_open())
    {
        configFile >> folderPath >> interval;
        syslog(LOG_INFO, "Read config.");
    }
    else
    {
        syslog(LOG_ERR, "Error: Config could't be open.");
        exit(EXIT_FAILURE);
    }
    if (configFile.eof())
    {
        syslog(LOG_ERR, "Error: Incorrect config file.");
        exit(EXIT_FAILURE);
    }

    configFile.close();
    CheckConfigValues();
}


Config::Config(char *confName)
{
    char* path;
    if ((path = realpath(confName, nullptr)) == nullptr)
    {
        syslog(LOG_ERR, "Error: Config doesn't exist.");
        exit(EXIT_FAILURE);
    }
    configPath = path;
    ReadConfig();
}


Config& Config::GetInstance(char *confName)
{
    static Config instance(confName);
    return instance;
}

const string& Config::GetFolderPath()
{
    return folderPath;
}

int Config::GetInterval()
{
    return interval;
}
