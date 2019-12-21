#ifndef GOAT_H_INCLUDED
#define GOAT_H_INCLUDED

#include "conn.h"

class Goat {
public:
	static Goat& GetInstance();
	void MakeMove();
	void FindDeadOrAlive();

private:
	Goat();
	Conn conn;

	int isAlive;

	const int GOAT_MIN = 1;
	const int ALIVE_MAX = 100;
	const int DEAD_MAX = 50;
};

#endif //GOAT_H_INCLUDED