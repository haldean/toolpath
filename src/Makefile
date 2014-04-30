BINARY=tp
TEST_BINARY=slicetest

CC=clang++
CFLAGS=-c -Wall -I../include --std=c++0x -I/usr/include/GL -I/usr/include -O2 -fvisibility=hidden -DGL_GLEXT_PROTOTYPES
LDFLAGS=-L/usr/local/lib -L/usr/X11/lib -L/usr/lib -lm -lglut -lGL -lGLU -lre2 -lmeshparse

SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
UNAME=$(shell uname)

ifdef DEBUG
CFLAGS+=-O0 -g -DDEBUG
endif

TEST_OBJS=$(filter-out main.o, $(OBJECTS))
MAIN_OBJS=$(filter-out slicetest.o, $(OBJECTS))

all: $(SOURCES) $(BINARY)

$(BINARY): $(MAIN_OBJS)
	$(CC) $(LDFLAGS) $(MAIN_OBJS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(BINARY) $(TEST_BINARY)

$(TEST_BINARY): $(TEST_OBJS)
	$(CC) $(LDFLAGS) $(TEST_OBJS) -o $@
