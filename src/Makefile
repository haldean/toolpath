BINARY=tp
TEST_BINARY=slicetest

CFLAGS=-c -Wall -I../include --std=c++11 -I/usr/include/GL -I/usr/include -O2 -fvisibility=hidden -DGL_GLEXT_PROTOTYPES
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
	$(CXX) $(MAIN_OBJS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CXX) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(BINARY) $(TEST_BINARY)

$(TEST_BINARY): $(TEST_OBJS)
	$(CXX) $(LDFLAGS) $(TEST_OBJS) -o $@
