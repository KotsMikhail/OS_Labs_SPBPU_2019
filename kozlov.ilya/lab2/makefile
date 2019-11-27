CC=g++
HOME=$(shell pwd)
INCLUDEDIRS=-I$(HOME)/interface
HOST_DIR=$(HOME)/wolf
HOST_SRC=$(shell ls $(HOST_DIR)/**/*.cpp)
HOST_OBJ_FULL=$(HOST_SRC:%.cpp=%.o)
HOST_OBJ=$(foreach obj, $(HOST_OBJ_FULL), $(shell basename $(obj)))	
CLIENT_DIR=$(HOME)/goat
CLIENT_SRC=$(shell ls $(CLIENT_DIR)/**/*.cpp)
CLIENT_OBJ_FULL=$(CLIENT_SRC:%.cpp=%.o)
CLIENT_OBJ=$(foreach obj, $(CLIENT_OBJ_FULL), $(shell basename $(obj)))
LIB_DIR=$(HOME)/conn
LDFLAGS=-L$(LIB_DIR) -lpthread -Wl,-rpath=$(LIB_DIR)
LIB_SOURCES=$(shell cd $(LIB_DIR) && ls conn_*.cpp)
POSTFIX=$(LIB_SOURCES:conn%.cpp=%)
CFLAGS=-c -std=c++11 -Wall -Werror -fPIC $(INCLUDEDIRS)

all: make_libs make_clients
	echo "Make hosts"
	for p in $(POSTFIX); do \
		$(CC) $(CFLAGS) -Dhost$$p -I$(HOST_DIR)/ $(HOST_SRC); \
		$(CC) $(HOST_OBJ) -o host$$p $(LDFLAGS) -lconn$$p; \
		rm -f *.o; \
	done

make_clients:
	for p in $(POSTFIX); do \
		$(CC) $(CFLAGS) -Dclient$$p -I$(CLIENT_DIR)/ $(CLIENT_SRC); \
		$(CC) $(CLIENT_OBJ) -o client$$p $(LDFLAGS) -lconn$$p; \
		rm -f *.o; \
	done

make_libs: 
	cd $(LIB_DIR) && \
	make

clean:
	cd $(LIB_DIR) && \
	make clean	
	rm -f client_* host_*
	rm -f *.so *.o