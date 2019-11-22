CC=g++
DIR=src
INCLUDEDIRS=\
	-I$(DIR)/ \
	-I../interface
POSTFIX=seg mq fifo
OBJECTS=client.o goat.o
LIB_DIR=$(shell pwd)/../conn/
LDFLAGS=-L$(LIB_DIR) -lrt -lpthread -Wl,-rpath=$(LIB_DIR)
CFLAGS=-c -Wall -Werror $(INCLUDEDIRS)

all: make_client
	cp client_* ../

make_client: client.o
	for p in $(POSTFIX); do \
		$(CC) $(OBJECTS) -o client_$$p $(LDFLAGS) -lconn_$$p; \
	done

client.o: goat.o
	$(CC) $(CFLAGS) $(DIR)/client.cpp

goat.o:
	$(CC) $(CFLAGS) $(DIR)/goat.cpp

clean:
	rm -f client_* *.o
