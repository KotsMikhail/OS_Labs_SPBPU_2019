//
// Created by ilya on 10/10/19.
//

#include "daemon.h"

#include <csignal>
#include <sys/stat.h>
#include <zconf.h>
#include <syslog.h>
#include <fstream>
#include <map>
#include <dirent.h>
#include <cstring>
#include <iomanip>

#include "parser.h"

// Daemon constants
const std::string Daemon::NAME = "LAB1 - DAEMON";
// Static members, default values
char* Daemon::config_file_ = nullptr;
unsigned int Daemon::time_interval_ = 30;
std::string Daemon::pid_file_ = "/var/run/lab1.pid";
std::string Daemon::dir1_ = "dir1";
std::string Daemon::dir2_ = "dir2";
std::string Daemon::home_dir_;
std::string Daemon::hist_log_;

// PUBLIC //

bool Daemon::Init(char* config_file)
{
  openlog(NAME.c_str(), LOG_PID | LOG_NDELAY, LOG_USER);
  config_file_ = realpath(config_file, nullptr);
  if (config_file_ == nullptr)
  {
    syslog(LOG_ERR, "Can't get full path from: %s", config_file);
    return false;
  }
  char buff[FILENAME_MAX];
  getcwd(buff, FILENAME_MAX);
  home_dir_ = buff;
  syslog(LOG_INFO, "Home dir is %s", buff);
  SetSignals();
  return LoadConfig();
}

void Daemon::Start()
{
  if (setsid() == -1)
  {
    syslog(LOG_ERR, "setsid returned error: %d", errno);
    Terminate();
  }
  pid_t pid = fork();
  if (pid == -1)
  {
    syslog(LOG_ERR, "second fork failed");
    Terminate();
  }
  if (pid == 0)
  {
    syslog(LOG_INFO, "Creating daemon-child...");
    umask(0);
    if (chdir("/") == -1) {
      syslog(LOG_ERR, "chdir returned error: %d", errno);
      Terminate();
    }
    if (close(STDIN_FILENO) == -1 ||
        close(STDOUT_FILENO) == -1 ||
        close(STDERR_FILENO) == -1) {
      syslog(LOG_ERR, "close returned error: %d", errno);
      Terminate();
    }
    StartWork();
  }
  syslog(LOG_INFO, "Exiting daemon-parent...");
}

void Daemon::Clear()
{
  free(config_file_);
  syslog(LOG_INFO, "Closing...");
  closelog();
}

void Daemon::Terminate()
{
  Clear();
  exit(SIGTERM);
}

// PRIVATE //

std::string Daemon::GetFullPath(std::string& path)
{
  if (path[0] == '/')
  {
    return path;
  }
  path = home_dir_ + "/" + path;
  char* real = realpath(path.c_str(), nullptr);
  if (real == nullptr)
  {
    syslog(LOG_ERR, "Can't get full path from: %s", path.c_str());
    return std::string();
  }
  std::string res(real);
  free(real);
  return res;
}

bool Daemon::LoadConfig()
{
  syslog(LOG_INFO, "Loading config...");
  std::ifstream in_file(config_file_);
  if (!in_file)
  {
    syslog(LOG_ERR, "Can't open config file");
    Clear();
    return false;
  }
  // Getting values from config
  std::map<Parser::ConfigName, std::string> config_dict = Parser::ParseFile(in_file);
  in_file.close();
  if (config_dict.size() == 1 && config_dict.find(Parser::ERROR) != config_dict.end())
  {
    syslog(LOG_ERR, "CONFIG ERROR: %s", config_dict.at(Parser::ERROR).c_str());
    Clear();
    return false;
  }
  if (config_dict.find(Parser::INTERVAL) != config_dict.end())
  {
    try
    {
      time_interval_ = static_cast<unsigned int>(std::stol(config_dict.at(Parser::INTERVAL)));
    }
    catch (std::exception &e) {
      syslog(LOG_ERR, "CONFIG ERROR: %s", e.what());
      Clear();
      return false;
    }
  }
  if (config_dict.find(Parser::DIR1) != config_dict.end())
  {
    dir1_ = config_dict.at(Parser::DIR1);
  }
  if (config_dict.find(Parser::DIR2) != config_dict.end())
  {
    dir2_ = config_dict.at(Parser::DIR2);
  }
  // Getting full paths
  dir1_ = GetFullPath(dir1_);
  if (dir1_.empty())
  {
    Clear();
    return false;
  }
  dir2_ = GetFullPath(dir2_);
  if (dir2_.empty())
  {
    Clear();
    return false;
  }
  hist_log_ = dir2_ + "/hist.log";
  syslog(LOG_INFO, "interval: %d, dir1: %s, dir2: %s",
      time_interval_, dir1_.c_str(), dir2_.c_str());
  return true;
}

bool Daemon::CheckPidFile()
{
  std::ifstream pid_file(pid_file_);
  if (pid_file)
  {
    pid_t other;
    pid_file >> other;
    pid_file.close();
    struct stat sb;
    std::string path_to_daemon = "/proc/" + std::to_string(other);
    if (stat(path_to_daemon.c_str(), &sb) == 0)
    {
      kill(other, SIGTERM);
    }
  }
  return SetPidFile();
}

bool Daemon::SetPidFile()
{
  std::ofstream out;
  out.open(pid_file_, std::ofstream::out | std::ofstream::trunc);
  if (!out.is_open())
  {
    syslog(LOG_ERR, "Can't open pid file...");
    return false;
  }
  out << getpid();
  out.close();
  return true;
}

void Daemon::SetSignals()
{
  signal(SIGTERM, SignalHandler);
  signal(SIGHUP, SignalHandler);
}

void Daemon::StartWork()
{
#ifdef PID_FILE
  pid_file_ = PID_FILE;
#endif
  pid_file_ = GetFullPath(pid_file_);
  if (pid_file_.empty() || !CheckPidFile())
  {
    Terminate();
  }
  syslog(LOG_INFO, "pid_file: %s", pid_file_.c_str());
  syslog(LOG_INFO, "Starting...");
  while (true)
  {
    DoWork();
    sleep(time_interval_);
  }
}

void Daemon::DoWork() {
  std::ofstream hist_file;
  hist_file.open(hist_log_, std::ostream::out | std::ostream::app);
  if (hist_file.is_open())
  {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X\n", &tstruct);
    hist_file << buf;
    ListDirRec(hist_file, dir1_, 0);
    hist_file.close();
  }
}

void Daemon::ListDirRec(std::ofstream& hist_log, std::string& path, int indent)
{
  DIR *dir;
  struct dirent *entry;
  if (!(dir = opendir(path.c_str())))
  {
    syslog(LOG_ERR, "Can't open %s", path.c_str());
    return;
  }
  while ((entry = readdir(dir)) != nullptr)
  {
    if (entry->d_type == DT_DIR)
    {
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      {
        continue;
      }
      std::string new_path = path + "/" + std::string(entry->d_name);
      for (int i = 0; i < indent; i++)
      {
        hist_log << " ";
      }
      hist_log << entry->d_name << "/" << std::endl;
      ListDirRec(hist_log, new_path, indent + 2);
    } else {
      for (int i = 0; i < indent; i++)
      {
        hist_log << " ";
      }
      hist_log << entry->d_name << std::endl;
    }
  }
  closedir(dir);
}

void Daemon::SignalHandler(int sig_num)
{
  if (sig_num == SIGTERM)
  {
    Terminate();
  }
  else if (sig_num == SIGHUP)
  {
    if (!LoadConfig())
    {
      Terminate();
    }
  }
}
