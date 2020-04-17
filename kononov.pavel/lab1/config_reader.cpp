#include "config_reader.h"

#include <fstream>

void ConfigReader::read()
{
	std::ifstream config = std::ifstream(config_path);
	if (!config.is_open() || config.eof()) {
        	syslog(LOG_ERR, "Bad path or empty config");
        	exit(EXIT_FAILURE);
    	}
	
	std::string line;
	config >> line;

	wt = -1;
	config >> wt;

	DIR *dp;
	dp = opendir(line.c_str());

	if (wt <= 0.0)
	{
		syslog(LOG_NOTICE, "Use default wait time equals 120 sec");
		wt = 120.0;
	}

	if (dp == NULL) 
	{
		syslog(LOG_NOTICE, "Path from config not exist");
		exit(EXIT_FAILURE);
	}
		
	dir_path = line;
	closedir(dp);
	
	config.close();
}
