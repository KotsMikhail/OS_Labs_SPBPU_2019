TEST_OBJECT_FILES = test.o

CFLAGS = -Wall -Werror -std=c++11
LIBS = -lpthread -latomic
CC = g++  

all: test
test: $(TEST_OBJECT_FILES) 

test:
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@   

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< 

clean: 
	rm -rf test *.o *~ 