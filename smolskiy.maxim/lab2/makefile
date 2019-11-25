all: build_all

define build
	g++ -std=c++11 -Wall -Werror -I ./int/ ./host/* ./int/* ./conns/conn_$(1).cpp -o host_$(1) -pthread -lrt;
endef

build_all:
	$(foreach conn,$(wildcard $(shell find ./conns/ -name '*.cpp')),$(call build,$(conn:./conns/conn_%.cpp=%)))

clean:
	rm -f host_*

