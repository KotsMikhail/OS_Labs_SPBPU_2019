# CFLAGS = -std=c++11
# PFLAGS = -pthread -fopenmp -lrt
#
# all:
# 	g++ host.cpp conn_shm.cpp wolf.cpp wolf.h conn.h goat.cpp goat.h Message.h connection_info.h -o lab2_shm $(PFLAGS) ;

all: build_all

define build
	g++ -std=c++11 -Wall -Werror -I ./interfaces/ -Dhost_$(1) ./host/* ./interfaces/* ./connections/conn_$(1).cpp -o host_$(1) -pthread -lrt;
endef

build_all:
	$(foreach conn,$(wildcard $(shell find ./connections/ -name '*.cpp')),$(call build,$(conn:./connections/conn_%.cpp=%)))

clean:
	rm -f host_*