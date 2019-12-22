COMPILE_FLAGS = -std=c++11 -Wall -Werror -I ./headers/ -Wextra -pedantic-errors
PFLAGS = -pthread

all: build_test

build_test:
	@g++ $(COMPILE_FLAGS) ./src/* -o Lab3_set $(PFLAGS)

clean:
	@rm -f Lab3_set
