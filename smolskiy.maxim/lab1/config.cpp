#include <syslog.h>

#include <fstream>
#include <limits.h>

#include "config.h"

using namespace std;

const string& Config::GetAbsoluteFolderPath()
{
    return absfldrPath;
}

int Config::GetInterval()
{
    return interval;
}

void Config::ReadConfig()
{
    ifstream cfgFile(absPath);
    cfgFile >> absfldrPath >> interval;
    
    if (cfgFile.eof())
    {
        syslog(LOG_ERR, "Bad config.");
        exit(EXIT_FAILURE);
    }
    
    cfgFile.close();
    syslog(LOG_INFO, "Read config.");
}

Config::Config(char *cfgName)
{
    char buf[PATH_MAX];
    
    if (realpath(cfgName, buf) == nullptr)
    {
        syslog(LOG_ERR, "Config doesn't exist.");
        exit(EXIT_FAILURE);
    }
    
    absPath = buf;
    ReadConfig();
}

Config& Config::GetInstance(char *cfgName)
{
    static Config instance(cfgName);
    return instance;
}
