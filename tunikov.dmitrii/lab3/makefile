CFLAGS = -std=c++11 -Wall -Werror
LIBS = -lpthread
CC = g++
NAME = stack_lab

build_all:
	$(CC) $(CFLAGS) main.cpp ./*/*.cpp ./*/*.h -o $(NAME) $(LIBS)

clean:
	rm -f $(NAME)*