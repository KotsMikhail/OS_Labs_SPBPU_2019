HOME_DIR = $(shell pwd)
HOST_DIR = $(HOME_DIR)/wolf
CLIENT_DIR = $(HOME_DIR)/goat
CONN_SEG_DIR = $(HOME_DIR)/conn_seg
CONN_MQ_DIR = $(HOME_DIR)/conn_mq
#CONN_FIFO_DIR = $(HOME_DIR)/conn_fifo

all: build_libs build_clients
	cd $(HOST_DIR) && \
	./build.sh
	echo "Build done"

build_libs:
	cd $(CONN_SEG_DIR) && \
	./build.sh
	cd $(CONN_MQ_DIR) && \
	./build.sh

build_clients:
	cd $(CLIENT_DIR) && \
	./build.sh	

clean: rm_host rm_client rm_lib
	echo "Build cleaned"

rm_host:
	rm -f host_* **/host_*

rm_client:
	rm -f client_* **/client_*

rm_lib:
	rm -f **/*.so
