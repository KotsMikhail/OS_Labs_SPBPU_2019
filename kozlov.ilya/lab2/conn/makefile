CC=g++
INCLUDEDIRS=-I../interface
LDFLAGS=-shared -lrt
SOURCES=$(shell ls conn_*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
NAMES=$(OBJECTS:%.o=%)
CFLAGS=-c -std=c++11 -Wall -Werror -fPIC $(INCLUDEDIRS)

all: make_libs

make_libs: $(OBJECTS)
	echo $(NAMES)
	for name in $(NAMES); do \
	    $(CC) $$name.o $(LDFLAGS) -o lib$$name.so; \
	done

.cpp.o:
	$(CC) $(CFLAGS) $<

clean:
	rm -f *.so *.o
