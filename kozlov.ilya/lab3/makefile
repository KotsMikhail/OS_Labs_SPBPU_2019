LDFLAGS = -lpthread
GCC = g++
SRC_DIR = ./src
OPT_LIST_DIR = $(SRC_DIR)/optimistic_list
LAZY_LIST_DIR = $(SRC_DIR)/lazy_list
SHARED_DIR = $(SRC_DIR)/shared
CFLAGS = -std=c++11 -Wall -Werror -I$(SHARED_DIR) -DLOG_DEBUG

define build
	$(GCC) $(CFLAGS) $(LAZY_LIST_DIR)/*.h $(OPT_LIST_DIR)/*.h $(SHARED_DIR)/**/*.h \
	    $(SHARED_DIR)/**/*.cpp $(SRC_DIR)/main.cpp -o tests $(LDFLAGS);
	echo "done"
endef

all:
	$(call build)

clean:
	@rm -f tests