TEST_OBJECT_FILES = test.o

CFLAGS = -Wall -std=c++11 -I
LIBS = -lpthread -latomic
CC = g++  
NAME = Lab_3(stack)


build_all:
	$(CC) $(CFLAGS) ./src/ ./src/*.cpp ./src/*.h  -o $(NAME) $(LIBS)

clean: 
	rm -f $(NAME)*
