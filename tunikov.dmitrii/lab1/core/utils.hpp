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
        static std::string getAbsolutePath(std::string path)
        {
            if (path.empty())
            {
                throw CommonException("can't get absolute path, path is empty");
            }
            if (path[0] == '/')
            {
                return path;
            }

            char *abs_path = realpath(path.c_str(), nullptr);
            if (abs_path == nullptr)
            {
                throw CommonException("can't get absolute path: " + path);
            }

            std::string res = std::string(abs_path);
            delete abs_path;
            return res;
        }

        static void createPidFile(const std::string& pid_file_name)
        {
            //open for output and clear old data
            ofstream pid_file(pid_file_name, std::ofstream::out | std::ofstream::trunc);
            if (!pid_file.is_open())
                throw CommonException("can't open pid file: " + pid_file_name);

            pid_file << getpid();
            pid_file.close();
        }
        static void updatePidFile()
        {
            static std::string pid_file_name = "/var/run/inotify.pid";

#ifdef PID_FILE_NAME
    pid_file_name = PID_FILE_NAME;
#endif
            std::string pid_abs = utils::pidFile::getAbsolutePath(pid_file_name);
            syslog(LOG_LOCAL0, "pid file location is %s", pid_abs.c_str());

            ifstream pid_file(utils::pidFile::getAbsolutePath(pid_file_name));
            if (!pid_file)
            {
                createPidFile(pid_file_name);
            }
            else
            {
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
