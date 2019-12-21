#ifndef WOLF_H_INCLUDED
#define WOLF_H_INCLUDED

#include "conn.h"

class Wolf {
public:
	static Wolf& GetInstance();
	bool CatchGoat();

private:
	Wolf();
	Conn conn;

	int cntMove;
	int cntDeadMove;
	int isAlive;

	const int WOLF_MIN = 1;
	const int WOLF_MAX = 100;
	const int DEAD_MOVE = 2;
	const int ALIVE_MAX = 70;
	const int DEAD_MAX = 20;
};

#endif // WOLF_H_INCLUDED
