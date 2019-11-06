#include "daemon.h"


Daemon& Daemon::get_instance() 
{
    static Daemon instance;
    return instance;
}

Daemon::Daemon()
{
    PID_FILE = "/var/run/daemon_lab.pid";
}

void Daemon::hardcore_date_time_validate(std::string date, std::string time)
{
    if (date.length() > 10 || date.length() < 5 || time.length() > 8 || time.length() < 5)
    {
        //std::cout << "fail 5" << std::endl;
        syslog(LOG_ERR, "Wrong date or time");
        exit(EXIT_FAILURE);
    }

    if (date.find('+') != std::string::npos || time.find('+') != std::string::npos || date.find('-') != std::string::npos || time.find('-') != std::string::npos)
    {
        //std::cout << "fail 6" << std::endl;
        syslog(LOG_ERR, "Wrong date or time");
        exit(EXIT_FAILURE);
    }

    std::replace( date.begin(), date.end(), '.', ' ');
    std::replace( time.begin(), time.end(), ':', ' ');

    std::istringstream date_iss(date);
    std::vector<std::string> date_results(std::istream_iterator<std::string>{date_iss},
                                 std::istream_iterator<std::string>());

    std::istringstream time_iss(time);
    std::vector<std::string> time_results(std::istream_iterator<std::string>{time_iss},
                                 std::istream_iterator<std::string>());

    if (atoi(date_results[0].c_str()) > 31 || atoi(date_results[0].c_str()) < 1 || atoi(date_results[1].c_str()) > 12 || atoi(date_results[1].c_str()) < 1 || atoi(time_results[0].c_str()) > 23 || atoi(time_results[1].c_str()) > 59 || atoi(time_results[2].c_str()) > 59)
    {
        //std::cout << "fail 7" << std::endl;
        syslog(LOG_ERR, "Wrong date or time");
        exit(EXIT_FAILURE);
    }
}

void Daemon::read_config()
{
    std::string msg_text;
    std::string msg_flag;
    std::string msg_date;
    std::string msg_time;

    std::ifstream cfg_file(cfg_path);

    if (!cfg_file.is_open() || cfg_file.eof()) 
    {
        syslog(LOG_ERR, "Could not open config file or it is empty");
        exit(EXIT_FAILURE);
    }
    //std::cout << timeinfo->tm_sec << std::endl;
    cfg_data.clear();
    while (cfg_file >> msg_date >> msg_time >> msg_flag)
    {
        
        //std::cout << "process line" << std::endl;
        if (msg_flag.find('-') != std::string::npos && msg_flag.length() == 2 && !(msg_flag.compare("-h") == 0 || msg_flag.compare("-d") == 0 || msg_flag.compare("-w") == 0 || msg_flag.compare("-m") == 0))
        {
            std::cout << "fail 0" << std::endl;
            syslog(LOG_ERR, "Wrong flag");
            exit(EXIT_FAILURE);
        }

        if (msg_flag.compare("-h") == 0 || msg_flag.compare("-d") == 0 || msg_flag.compare("-w") == 0 || msg_flag.compare("-m") == 0)
        {
            if (!(cfg_file >> msg_text))
            {
                //std::cout << "fail 1" << std::endl;
                syslog(LOG_ERR, "Wrong confog format");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            msg_text = msg_flag;
            msg_flag = "-m";
        }

        struct tm t;
        memset(&t, 0, sizeof t);  
        if (3 != sscanf(msg_date.c_str(),"%d.%d.%d", &t.tm_mday, &t.tm_mon, &t.tm_year)) 
        {
            //std::cout << "fail 2" << std::endl;
            syslog(LOG_ERR, "Wrong date format");
            exit(EXIT_FAILURE);
        }
        if (3 != sscanf(msg_time.c_str(),"%d:%d:%d", &t.tm_hour, &t.tm_min, &t.tm_sec)) 
        {
            //std::cout << "fail 3" << std::endl;
            syslog(LOG_ERR, "Wrong time format");
            exit(EXIT_FAILURE);
        }

        hardcore_date_time_validate(msg_date, msg_time);
            
        t.tm_year -= 1900;
        t.tm_mon--;
        
        if (mktime(&t) < 0) 
        {
            //std::cout << "fail 4" << std::endl;
            syslog(LOG_ERR, "Wrong date-time format");
            exit(EXIT_FAILURE);
        }

        

        cfg_entry entry(msg_text, msg_flag, t);
        cfg_data.push_back(entry);
        //printf("pepz: %d %d %d %d %d %d\n", t.tm_mday, t.tm_mon, t.tm_year, t.tm_hour, t.tm_min, t.tm_sec);
    }

    cfg_file.close();
}

void Daemon::process_config_file(int interval) 
{
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    //std::cout << timeinfo->tm_sec << std::endl;
    

    for (std::list<cfg_entry>::iterator it = cfg_data.begin(); it != cfg_data.end(); it++)
    {
        //printf("today: %d %d %d %d %d %d\n", timeinfo->tm_mday, timeinfo->tm_mon, timeinfo->tm_year, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
        //printf("mgday: %d %d %d %d %d %d\n", it->get_entry_time().tm_mday, it->get_entry_time().tm_mon, it->get_entry_time().tm_year, it->get_entry_time().tm_hour, it->get_entry_time().tm_min, it->get_entry_time().tm_sec);
        tm tmp_t = it->get_entry_time();
        time_t today_time = mktime(timeinfo);
        time_t msg_time = mktime(&tmp_t);
        //printf("today: %lld\nmgday: %lld\n", (long long)t1, (long long)t2);
        if(today_time < msg_time) //no need to output future reminders
            continue;
        
        if (it->get_entry_flag().compare("-m") == 0)
        {
            //system("gnome-terminal -e 'echo \"sas\"'");//(std::string("gnome-terminal -- 'grep -o \"") + msg_text + std::string("\" ~/.conf'")).c_str());
            //system("gnome-terminal  echo sas");
            
            //system("xterm -e echo sas");
            //std::cout << "sas1" << std::endl;
            if (std::abs(timeinfo->tm_sec - tmp_t.tm_sec) <= interval)
            {
                system(std::string(("gnome-terminal --working-directory='/home' -- sh -c \"echo ") + it->get_entry_text() + std::string("; read line\"")).c_str());//output msg_text
            }
        }
        else if (it->get_entry_flag().compare("-h") == 0)
        {
            if (timeinfo->tm_min == tmp_t.tm_min && std::abs(timeinfo->tm_sec - tmp_t.tm_sec) <= interval) 
            {
                system(std::string(("gnome-terminal --working-directory='/home' -- sh -c \"echo ") + it->get_entry_text() + std::string("; read line\"")).c_str());//output msg_text
            }
        }
        else if (it->get_entry_flag().compare("-d") == 0)
        {
            if (timeinfo->tm_hour == tmp_t.tm_hour && timeinfo->tm_min == tmp_t.tm_min && std::abs(timeinfo->tm_sec - tmp_t.tm_sec) <= interval) 
            {
                system(std::string(("gnome-terminal --working-directory='/home' -- sh -c \"echo ") + it->get_entry_text() + std::string("; read line\"")).c_str());//output msg_text
            }
        }
        else if (it->get_entry_flag().compare("-w") == 0)
        {
            if (timeinfo->tm_wday == tmp_t.tm_wday && timeinfo->tm_hour == tmp_t.tm_hour && timeinfo->tm_min == tmp_t.tm_min && std::abs(timeinfo->tm_sec - tmp_t.tm_sec) <= interval)
            {
                system(std::string(("gnome-terminal --working-directory='/home' -- sh -c \"echo ") + it->get_entry_text() + std::string("; read line\"")).c_str());//output msg_text
            }
        }
    }
}

void Daemon::kill_prev_daemon() 
{
    //std::cout << "kill prev" << std::endl;
    std::ifstream ipidFile(PID_FILE);
    if (ipidFile.is_open() && !ipidFile.eof())
    {
        pid_t prev;
        ipidFile >> prev;
        if (prev > 0) 
        {
            kill(prev, SIGTERM);
        }
    }
    ipidFile.close();
}

void Daemon::set_pid_file() 
{
    std::ofstream pid_file(PID_FILE);
    if (!pid_file) 
    {
        syslog(LOG_ERR, "Could not open pid file");
        exit(EXIT_FAILURE);
    }
    pid_file << getpid();
    pid_file.close();
}

void Daemon::signal_handler(int sig)
{
    switch(sig)
    {
        case SIGHUP:
            get_instance().read_config();
            syslog(LOG_NOTICE, "Hangup Signal Catched");
            break;
        case SIGTERM:
            syslog(LOG_NOTICE, "Terminate Signal Catched");
            unlink(get_instance().PID_FILE);
            exit(EXIT_SUCCESS);
            break;
        default:
            syslog(LOG_NOTICE, "Unknown Signal Catched");
            break;
    }
}