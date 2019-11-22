CC=g++
DIR=src
INCLUDEDIRS=\
	-I$(DIR)/ \
	-I$(shell pwd)/../interface
POSTFIX=seg mq fifo
OBJECTS=wolf.o host.o
LIB_DIR=$(shell pwd)/../conn/
LDFLAGS=-L$(LIB_DIR) -lrt -lpthread -Wl,-rpath=$(LIB_DIR)
CFLAGS=-c -Wall -Werror $(INCLUDEDIRS)

all: make_host
	cp host_* ../

make_host: host.o
	for p in $(POSTFIX); do \
		$(CC) $(OBJECTS) -o host_$$p $(LDFLAGS) -lconn_$$p; \
	done

host.o: wolf.o
	$(CC) $(CFLAGS) $(DIR)/host.cpp

wolf.o:  
	$(CC) $(CFLAGS) $(DIR)/wolf.cpp

clean:
	rm -f host_* *.o
