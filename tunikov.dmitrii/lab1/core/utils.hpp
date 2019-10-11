//
// Created by dmitrii on 04.10.2019.
//

#ifndef INOTIFY_UTILS_HPP
#define INOTIFY_UTILS_HPP

#include <unistd.h>
#include <climits>
#include <iostream>
#include <fstream>

namespace utils {
    namespace pidFile{
        static void createPidFile(const std::string& pid_file_name)
        {
            //open for output and clear old data
            ofstream pid_file(pid_file_name, std::ofstream::out | std::ofstream::trunc);
            if (!pid_file.is_open())
                throw CommonException("can't open pid file");

            pid_file << getpid();
            pid_file.close();
        }
        static void updatePidFile()
        {
            ConfigHolder *ch = ConfigHolder::getInstance();
            std::string pid_file_name = !ch->get("pid_file").empty() ?ch->get("pid_file")[0] : throw CommonException("can't find pid_file_name");

            ifstream pid_file(pid_file_name);
            if (!pid_file)
            {
                createPidFile(pid_file_name);
            } else{
                pid_t old_pid;
                pid_file >> old_pid;
                pid_file.close();

                struct stat sb;
                std::string path_to_old_pid = "/proc/" + std::to_string(old_pid);
                //kill old process
                if (stat(path_to_old_pid.c_str(), &sb) == 0)
                {
                    kill(old_pid, SIGTERM);
                }
                createPidFile(pid_file_name);
            }
        }
    }
}

#endif //INOTIFY_UTILS_HPP
