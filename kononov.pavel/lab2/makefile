CFLAGS = -std=c++11 -Wall -Werror -I ./interfaces/
PFLAGS = -pthread -lrt

CONNDIR = ./conn/
CONNPREFIX = $(CONNDIR)conn_
CONNECTIONS = $(shell find $(CONNDIR) -name '*.cpp')

all: build_hc

define build
	g++ $(CFLAGS) ./host/* ./utils/* ./interfaces/* $(CONNPREFIX)$(1).cpp -o host_$(1) $(PFLAGS) ;
	echo host_$(1) ;
	g++ $(CFLAGS) ./client/* ./utils/* ./interfaces/* $(CONNPREFIX)$(1).cpp -o client_$(1) $(PFLAGS) ;
	echo client_$(1) ;
endef

build_hc:
	@$(foreach conn,$(wildcard $(CONNECTIONS)),$(call build,$(conn:$(CONNPREFIX)%.cpp=%)))


clean:
	@rm -f host_* client_*
