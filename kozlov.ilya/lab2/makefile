CC=g++
HOME=$(shell pwd)
INCLUDEDIRS=-I$(HOME)/interface
HOST_DIR=$(HOME)/wolf
HOST_SRC=$(shell ls $(HOST_DIR)/**/*.cpp)
HOST_OBJ_FULL=$(HOST_SRC:%.cpp=%.o)
HOST_OBJ=$(foreach obj, $(HOST_OBJ_FULL), $(basename $(obj)))	
CLIENT_DIR=$(HOME)/goat
CLIENT_SRC=$(shell ls $(CLIENT_DIR)/**/*.cpp)
CLIENT_OBJ_FULL=$(CLIENT_SRC:%.cpp=%.o)
CLIENT_OBJ=$(foreach obj, $(CLIENT_OBJ_FULL), $(basename $(obj)))
LIB_DIR=$(HOME)/conn
LDFLAGS=-L$(LIB_DIR) -lrt -lpthread -Wl,-rpath=$(LIB_DIR)
LIB_SOURCES=$(shell cd $(LIB_DIR) && ls conn_*.cpp)
POSTFIX=$(LIB_SOURCES:conn%.cpp=%)
CFLAGS=-c -Wall -Werror -fPIC $(INCLUDEDIRS)

all: make_libs make_clients
	echo "Make hosts"
	$(CC) $(CFLAGS) -I$(HOST_DIR)/ $(HOST_SRC)
	for p in $(POSTFIX); do \
		$(CC) $(HOST_OBJ) -o host$$p $(LDFLAGS) -lconn$$p; \
	done

make_clients:
	$(CC) $(CFLAGS) -I$(CLIENT_DIR)/ $(CLIENT_SRC)
	for p in $(POSTFIX); do \
		$(CC) $(CLIENT_OBJ) -o client$$p $(LDFLAGS) -lconn$$p; \
	done

make_libs: 
	cd $(LIB_DIR) && \
	make

clean:
	cd $(LIB_DIR) && \
	make clean	
	rm -f client_* host_*
	rm -f *.so *.o