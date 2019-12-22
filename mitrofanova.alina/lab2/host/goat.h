#ifndef GOAT_H_INCLUDED
#define GOAT_H_INCLUDED

#include "conn.h"
#include "Semaphore.h"

class Goat {
public:
	static Goat& GetInstance();
	void Working(sem_t* hostSem, sem_t* clientSem);

private:
	Goat();
	Conn conn;
	void MakeMove();
	void FindDeadOrAlive();

	int isAlive;

	const int GOAT_MIN = 1;
	const int ALIVE_MAX = 100;
	const int DEAD_MAX = 50;
};

#endif //GOAT_H_INCLUDED