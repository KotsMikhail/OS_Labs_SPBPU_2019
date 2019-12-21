#include <cstdlib>
#include <ctime>
#include "goat.h"

Goat& Goat::GetInstance() {
	static Goat instance;
	return instance;
}

Goat::Goat() : conn(getppid(), false) {
	isAlive = 1;
	srand(time(nullptr));
}

void Goat::MakeMove() {
	// On each turn the Alive goat throws a number from 1 to 100
	// On each turn the Dead goat throws a number from 1 to 50
	if (isAlive) {
		conn.Write(rand() % ALIVE_MAX + GOAT_MIN);
	} else {
		conn.Write(rand() % DEAD_MAX + GOAT_MIN);
	}
}

void Goat::FindDeadOrAlive() {
	conn.Read(isAlive);
}