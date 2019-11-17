HOME_DIR = $(shell pwd)
HOST_DIR = $(HOME_DIR)/wolf
CONN_SEG_DIR = $(HOME_DIR)/conn_seg

all: build_libs
	cd $(HOST_DIR) && \
	./build.sh

build_libs:
	cd $(CONN_SEG_DIR) && \
	./build.sh

clean:
	rm -f $(HOST_DIR)/host_seg $(CONN_SEG_DIR)/*.so