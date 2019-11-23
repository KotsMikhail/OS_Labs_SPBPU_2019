CONNECTIONINTERFACES = ./connections/conn_interfaces/
COMPILEFLAGS = -std=c++11 -Wall -Werror -I $(CONNECTIONINTERFACES)
PFLAGS = -pthread -lrt

# some useful variables
CONNECTIONDIR = ./connections/src/
CONNECTIONPREFIX = $(CONNECTIONDIR)conn_

# find all 'conn_*.cpp' files
CONNECTIONFILES = $(shell find $(CONNECTIONDIR) -name '*.cpp')

all: build_all

define build_one_connection
	echo building host_$(1) ... ;
	g++ $(COMPILEFLAGS) -DPOSTFIX="$(1)" ./host/* $(CONNECTIONINTERFACES)* $(CONNECTIONPREFIX)$(1).cpp -o host_$(1) $(PFLAGS) ;
	echo building host_$(1) success ;

	echo building client_$(1) ... ;
	g++ $(COMPILEFLAGS) -DPOSTFIX="$(1)" ./client/* $(CONNECTIONINTERFACES)* $(CONNECTIONPREFIX)$(1).cpp -o client_$(1) $(PFLAGS) ;
	echo building client_$(1) success ;
endef

build_all:
	@$(foreach connfile,$(wildcard $(CONNECTIONFILES)),$(call build_one_connection,$(connfile:$(CONNECTIONPREFIX)%.cpp=%)))

clean:
	@rm -f host_* client_*
