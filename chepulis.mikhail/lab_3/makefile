CFLAGS = -Wall -std=c++11 -I -Werror
LIBS = -lpthread -latomic
CC = g++
NAME = Lab_3_stack

build_all:
	$(CC) $(CFLAGS) ./src/*.cpp ./src/*.h -o $(NAME) $(LIBS)

clean: 
	rm -f $(NAME)*
