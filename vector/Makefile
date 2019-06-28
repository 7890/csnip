CC=gcc
CFLAGS=
CFLAGS+=-DDEBUG_ON

all: example

example: example.c vector.h vector.o
	$(CC) -o example example.c vector.o $(CFLAGS)

vector.o: vector.c vector.h
	$(CC) -c -o vector.o vector.c $(CFLAGS)

clean:
	-rm -f example
	-rm -f vector.o
