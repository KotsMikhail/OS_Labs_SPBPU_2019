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

#include "parser.h"

// Daemon constants
const std::string Daemon::DELIM = " :=";
const std::string Daemon::INTERVAL_KEY = "interval";
const std::string Daemon::PID_FILE_KEY = "pid_file";
const std::string Daemon::DIR1_KEY = "dir1";
const std::string Daemon::DIR2_KEY = "dir2";
// Static members, default values
char* Daemon::config_file_ = nullptr;
int Daemon::time_interval_ = 30;
std::string Daemon::pid_file_ = "/var/run/lab1.pid";
std::string Daemon::name_ = "LAB1 - DAEMON";
std::string Daemon::dir1_ = "dir1";
std::string Daemon::dir2_ = "dir2";

// PUBLIC //

bool Daemon::Init(char* config_file)
{
  openlog(name_.c_str(), LOG_PID | LOG_NDELAY, LOG_LOCAL0);
  config_file_ = realpath(config_file, nullptr);
  if (config_file_ == nullptr)
  {
    syslog(LOG_ERR, "Can't get full path from: %s", config_file);
    return false;
  }
  SetSignals();
  return LoadConfig();
}

void Daemon::Start()
{
  umask(0);
  setsid();
  chdir("/");
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
  StartWork();
}

void Daemon::Terminate()
{
  free(config_file_);
  syslog(LOG_INFO, "Closing...");
  closelog();
  exit(SIGTERM);
}

// PRIVATE //

std::string Daemon::GetFullPath(std::string& path)
{
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
    syslog(LOG_ERR, "Can't open file");
    return false;
  }
  std::map<std::string, std::string> config_dict = ParseFile(in_file, DELIM);
  in_file.close();
  if (config_dict.size() == 1 && config_dict.find(PARSER_ERROR) != config_dict.end())
  {
    syslog(LOG_ERR, "CONFIG ERROR: words are %s", config_dict.at(PARSER_ERROR).c_str());
    return false;
  }
  if (config_dict.find(INTERVAL_KEY) != config_dict.end())
  {
    try
    {
      time_interval_ = std::stoi(config_dict.at(INTERVAL_KEY));
    }
    catch (std::exception &e) {
      syslog(LOG_ERR, "CONFIG ERROR: %s", e.what());
      return false;
    }
  }
  if (config_dict.find(PID_FILE_KEY) != config_dict.end())
  {
    pid_file_ = config_dict.at(PID_FILE_KEY);
  }
  if (config_dict.find(DIR1_KEY) != config_dict.end())
  {
    dir1_ = config_dict.at(DIR1_KEY);
  }
  if (config_dict.find(DIR2_KEY) != config_dict.end())
  {
    dir2_ = config_dict.at(DIR2_KEY);
  }
  CheckPidFile();
  pid_file_ = GetFullPath(pid_file_);
  if (pid_file_.empty())
  {
    return false;
  }
  dir1_ = GetFullPath(dir1_);
  if (dir1_.empty())
  {
    return false;
  }
  dir2_ = GetFullPath(dir2_);
  if (dir2_.empty())
  {
    return false;
  }
  syslog(LOG_INFO, "interval: %d, pid_file: %s, dir1: %s, dir2: %s",
      time_interval_, pid_file_.c_str(), dir1_.c_str(), dir2_.c_str());
  return true;
}

void Daemon::CheckPidFile()
{
  std::ifstream pid_file(pid_file_);
  if (!pid_file)
  {
    SetPidFile();
  }
  else
  {
    pid_t other;
    pid_file >> other;
    pid_file.close();
    // TODO: search in '/proc'
    kill(other, SIGTERM);
    SetPidFile();
  }
}

void Daemon::SetPidFile()
{
  syslog(LOG_INFO, "Setting pid file...");
  std::ofstream out;
  out.open(pid_file_, std::ofstream::out | std::ofstream::trunc);
  if (!out.is_open())
  {
    syslog(LOG_ERR, "Can't create pid file...");
    return;
  }
  out << getpid();
  out.close();
}

void Daemon::SetSignals()
{
  signal(SIGTERM, SignalHandler);
  signal(SIGHUP, SignalHandler);
}

void Daemon::StartWork()
{
  syslog(LOG_INFO, "Starting...");
  while (true)
  {
    DoWork();
    sleep(time_interval_);
  }
}

void Daemon::DoWork()
{
  // TODO: main work with dirs
  syslog(LOG_INFO, "Working...");
}

void Daemon::SignalHandler(int sig_num)
{
  if (sig_num == SIGTERM)
  {
    Terminate();
  }
  else if (sig_num == SIGHUP)
  {
    LoadConfig();
  }
}
