PROJECT_DIR := $(PWD)
HOST_DIR := $(PROJECT_DIR)/host
CLIENT_DIR := $(PROJECT_DIR)/client
CONNECTIONS_DIR := $(PROJECT_DIR)/connections

SRC_FILES := $(wildcard **/*.cpp)

CONN_FILES := $(notdir $(wildcard $(CONNECTIONS_DIR)/*.cpp))
SUFFIXES := $(subst conn_, build_, $(CONN_FILES))
SUFFIXES := $(subst .cpp,;, $(SUFFIXES))
BUILD_TARGETS := $(SUFFIXES:;=)

OBJ_FILES := $(patsubst %.cpp, %.o, $(SRC_FILES))
HOST_OBJS :=$(patsubst %.cpp, %.o, $(wildcard host/*.cpp))
CLIENT_OBJS :=$(patsubst %.cpp, %.o, $(wildcard client/*.cpp))

all: $(OBJ_FILES) $(BUILD_TARGETS) clean

%.o: %.cpp
	g++ -std=c++0x -Wall -Werror -o $@ -c $<
	
build_%:
	g++ -std=c++0x -Wall -Werror $(HOST_OBJS) $(CONNECTIONS_DIR)/$(subst build_,conn_,$@).o -o $(subst build_,host_,$@) -lpthread -lrt
	g++ -std=c++0x -Wall -Werror $(CLIENT_OBJS) $(CONNECTIONS_DIR)/$(subst build_,conn_,$@).o -o $(subst build_,client_,$@) -lpthread -lrt

clean:
	rm -f $(OBJ_FILES)
