BINARY=tp
CFLAGS=-c -Wall -I../include/ --std=c++0x -I/usr/include/GL -I/usr/include -O2
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
UNAME=$(shell uname)
CC=clang++

ifeq ($(UNAME), Darwin)
CFLAGS+=-arch x86_64
LDFLAGS=-lm -framework OpenGL -framework GLUT
else
CFLAGS+=-DGL_GLEXT_PROTOTYPES
LDFLAGS=-L/usr/local/lib -L/usr/X11/lib -lm -lglut -lGL -lGLU
endif

all: $(SOURCES) $(BINARY)
	
$(BINARY): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(EXTRA_FLAGS) $(CFLAGS) $< -o $@

clean:
	rm -f ${OBJECTS} ${BINARY}
