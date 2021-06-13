# Makefile

CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0` -Wall -O3
LDLIBS = `pkg-config --libs gtk+-3.0`

DEPS = tools.h
OBJ = drawing.o tools.o

all: drawing

drawing: drawing.o tools.o

drawing.o: drawing.c

tools.o: tools.c

.PHONY: clean

clean:
	${RM} drawing
	${RM} *.o

# END
