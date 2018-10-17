all: Example

CXXFLAGS=-Wall -O2 -g
LOADLIBES= -lglut -lGL -lGLU -lXmu -lX11 -lm -L/usr/X11R6/lib
CC=g++

Example: Example.o Gloo.a

clean:
	-rm -f Example.o Example
