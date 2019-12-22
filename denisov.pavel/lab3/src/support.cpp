#include <pwd.h>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>

#include "support.h"

static std::string homeDirPath("");


void initHomeDir (void)
{
    passwd *pwd;
    uid_t uid;

    uid = geteuid();
    pwd = getpwuid(uid);
    if (pwd) {
        homeDirPath = std::string("/home/") + pwd->pw_name;
    } else {
        std::cout << "Couldn't find user with UID " << uid << ". Paths that beginning by `~` may not work." << std::endl;
    }
}


std::string getRealPath (std::string &path)
{
    if (!path.length()) {
        std::cout << "Couldn't find real path for empty string." << std::endl;
        return path;
    }

    if (path[0] == '~') {
        path.replace(0, 1, homeDirPath);
    }

    char *canonPath = realpath(path.c_str(), nullptr);
    if (!canonPath) {
        std::cout << "Couldn't real path for \"" << path << "\"";
        return std::string("");
    }

    std::string result(canonPath);
    free(canonPath);
    return result;
}


bool isDir (const std::string &path)
{
    struct stat st;
    return !stat(path.c_str(), &st) && S_ISDIR(st.st_mode);
}


bool fromStrToInt (const std::string &str, int &res)
{
    try {
        res = std::stoul(str);
    } catch (std::exception &e) {
        std::cout << "Couldn't convert string to int, exception - " << e.what() << std::endl;
        return false;
    }

    return true;
}
