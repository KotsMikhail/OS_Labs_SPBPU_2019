CFLAGS = -std=c++11 -Wall -Werror -I ./interfaces/
PFLAGS = -pthread -lrt

CONN_LIBS_DIR = ./conn/
CONN_PREFIX = $(CONN_LIBS_DIR)conn_
CONN_CPP = $(shell find $(CONN_LIBS_DIR) -name '*.cpp')

all: build_sources

define build
	#build host
	g++ $(CFLAGS) -Dhost_$(1) ./host/* ./interfaces/* $(CONN_PREFIX)$(1).cpp -o host_$(1) $(PFLAGS);
	echo host_$(1) was builded;

	#build client
	g++ $(CFLAGS) -Dclient_$(1) ./client/* ./interfaces/* $(CONN_PREFIX)$(1).cpp -o client_$(1) $(PFLAGS);
	echo client_$(1) was builded;
endef

build_sources:
	@$(foreach conn,$(wildcard $(CONN_CPP)),$(call build,$(conn:$(CONN_PREFIX)%.cpp=%)))

clean:
	@rm -f host_* client_*
