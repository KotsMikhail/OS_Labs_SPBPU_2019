LDFLAGS = -lpthread
GCC = g++
SRC_DIR = ./src
SHARED_DIR = $(SRC_DIR)/shared
TEST_DIR = $(SRC_DIR)/test
CFLAGS = -std=c++11 -Wall -Werror -I$(SHARED_DIR) ${ARGS}
NAME = tests
SOURCES = $(shell find . -name "*.h" -o -name "*.cpp")

all:
	$(GCC) $(CFLAGS) $(SOURCES) -o $(NAME) $(LDFLAGS);
	echo "done"

clean:
	rm -f $(NAME)