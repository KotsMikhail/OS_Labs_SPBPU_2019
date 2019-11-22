CFLAGS = -std=c++11 -Wall -Werror -I ./interfaces/
PFLAGS = -pthread -lrt

CONNDIR = ./conn/
CONNPREFIX = $(CONNDIR)conn_
CONNECTIONS = $(shell find $(CONNDIR) -name '*.cpp')

all: set_log build_hc

define build
	g++ $(CFLAGS) ./host/* ./interfaces/* $(CONNPREFIX)$(1).cpp -o host_$(1) $(PFLAGS) ;
	g++ $(CFLAGS) ./client/* ./interfaces/* $(CONNPREFIX)$(1).cpp -o client_$(1) $(PFLAGS) ;
endef

build_hc:
	@$(foreach conn,$(wildcard $(CONNECTIONS)),$(call build,$(conn:$(CONNPREFIX)%.cpp=%)))

set_log:
	@./set_log.sh

clean:
	@rm -f host_* client_*