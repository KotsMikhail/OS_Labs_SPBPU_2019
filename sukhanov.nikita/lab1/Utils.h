#ifndef __UTILS__
#define __UTILS__

#include <syslog.h>
#include <sys/stat.h>
#include <signal.h>
#include <dirent.h>
#include <unistd.h>

#include <iostream>
#include <fstream>

class Utils
{
public:
    static void FileCopy(const std::string &from, const std::string &to);

    static bool FolderExists(const std::string &path);

    static void CreateFolder(const std::string &path);

    static DIR *OpenFolder(const std::string &path);

    static void ClearFolder(const std::string &path);
};

#endif