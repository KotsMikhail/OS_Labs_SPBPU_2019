all: build_all

define build
	g++ -std=c++11 -Wall -Werror -I ./interface/ ./host/* ./client/* ./interface/* ./conn/conn_$(1).cpp -o host_$(1) -pthread -lrt;
endef

build_all:
	$(foreach conn,$(wildcard $(shell find ./conn/ -name '*.cpp')),$(call build,$(conn:./conn/conn_%.cpp=%)))

clean:
	rm -f host_*

