HOME_DIR = $(shell pwd)
HOST_DIR = $(HOME_DIR)/wolf
CLIENT_DIR = $(HOME_DIR)/goat
CONN_SEG_DIR = $(HOME_DIR)/conn_seg

all: build_libs build_clients
	cd $(HOST_DIR) && \
	./build.sh

build_libs:
	cd $(CONN_SEG_DIR) && \
	./build.sh

build_clients:
	cd $(CLIENT_DIR) && \
	./build.sh	

clean:
	rm -f host_seg *.so client_seg