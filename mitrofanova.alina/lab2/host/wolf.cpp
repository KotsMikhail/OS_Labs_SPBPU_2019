#include <iostream>
#include <syslog.h>
#include <iomanip>
#include <string>
#include "wolf.h"

Wolf& Wolf::GetInstance() {
	static Wolf instance;
	return instance;
}

void Wolf::Working(sem_t* hostSem, sem_t* clientSem) {
	bool isGameOver = false;

	do {
		Semaphore::TimedWait(hostSem);
		isGameOver = CatchGoat();
		Semaphore::Post(clientSem);
	} while (!isGameOver);
}

Wolf::Wolf() : conn(getpid(), true) {
	cntMove = 1;
	cntDeadMove = 0;
	isAlive = 1;

	std::cout << "THE GAME BEGAN" << std::endl;
}

bool Wolf::CatchGoat() {
	std::cout << "Move: " << std::setw(10) << cntMove << std::endl;

	int goatNum;
	conn.Read(goatNum);

	if (isAlive){
		std::cout << "Alive goat: " << std::setw(4) << goatNum << std::endl;
	} else {
		std::cout << "Dead goat : " << std::setw(4) << goatNum << std::endl;
	}

	// On each turn the wolf throws a number from 1 to 100
	int wolfNum = 0;
	std::cout << "Wolf number : "; 
	std::string str;
	bool is_correct = false;
	while (!is_correct) {
		std::getline(std::cin, str);
		if (str.empty() || str.find_first_not_of("0123456789") != std::string::npos) {
			std::cout << "Wrong input type" << std::endl;
		} else {
			try {
				wolfNum = std::stoi(str);
				if (wolfNum < 1 || wolfNum > 100) {
					std::cout << "Wrong range" << std::endl;
				} else {
					is_correct = true;
				}
			} catch (const std::invalid_argument& exp) {
				std::cout << "Not integer" << std::endl;
			}
		}
	}
	int diff = abs(wolfNum - goatNum);

	if (isAlive) {
		// if the number of alive goat differs from the number of a wolf by no more than 70,
		// then it is considered then goat hid and alive
		// else goat do not hide and are caught and considered dead
		if (diff <= ALIVE_MAX) {
			std::cout << "|" << std::setw(4) << goatNum << " - " << std::right << std::setw(4) << wolfNum << "|" << " <= " << std::setw(4) << ALIVE_MAX;
			std::cout << "    --> Goat is Alive" << std::endl;
		} else {
			isAlive = 0;
			std::cout << "|" << std::setw(4) << goatNum << " - " << std::right << std::setw(4) << wolfNum << "|" << " >  " << std::setw(4) << ALIVE_MAX;
			std::cout << "    -->  Goat is Dead" << std::endl;
		}
	} else {
		// if the number obtained by a dead goat differs from from the number of a wolf by no more than 20,
		// then goat is resurrected
		if (diff <= DEAD_MAX) {
			std::cout << "|" << std::setw(4) << goatNum << " - " << std::right << std::setw(4) << wolfNum << "|" << " <= " << std::setw(4) << DEAD_MAX;
			std::cout << "    --> Goat is Alive" << std::endl;
			isAlive = 1;
			cntDeadMove = 0;
		} else {
			std::cout << "|" << std::setw(4) << goatNum << " - " << std::right << std::setw(4) << wolfNum << "|" << " >  " << std::setw(4) << DEAD_MAX;
			std::cout << "    -->  Goat is Dead" << std::endl;
			cntDeadMove++;

			// the game ends if all the goats are dead for 2 consecutive turns
			if (cntDeadMove == DEAD_MOVE) {
				std::cout << "Two dead goats in a row. " << std::endl;
				std::cout << "THE GAME IS OVER" << std::endl;
				return true;
			}
		}
	}

	conn.Write(isAlive);
	cntMove++;

	return false;

}