#include "daemon.h"

int main(int argc, char** argv)
{
	Daemon instance = Daemon::getInstance();

	instance.init(argc, argv);
	instance.exec();

	return 0;
}
