#include "file_deleter.h"

void FileDeleter::exec() {
    std::time_t now = std::time(0);   // get time now
    struct stat sb;
    struct dirent *entry;
    DIR *dp;

    dp = opendir(path.c_str());
    if (dp == NULL) {
        syslog(LOG_ERR, "Could not open directory");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dp))) {
        std::string p = path;
        p += "/";
        p += entry->d_name;
        if (stat(p.c_str(), &sb) != 0) {
            syslog(LOG_ERR, "Could not open file");
            exit(EXIT_FAILURE);
        }
        double seconds = difftime(now, sb.st_ctime);
        if (seconds > treshold_seconds) {
            remove(p.c_str());
        }
    }

    closedir(dp);
}



