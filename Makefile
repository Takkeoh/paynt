# Makefile

CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0` -Wall -O3
LDLIBS = `pkg-config --libs gtk+-3.0`

DEPS = tools.h
OBJ = paynt.o tools.o

all: paynt

paynt: paynt.o tools.o

paynt.o: paynt.c

tools.o: tools.c

.PHONY: clean

clean:
	${RM} paynt
	${RM} *.o

# END
