CC=gcc
CFLAGS=-c -Wall
LIB_O=lib.o

output: main.o library.o
	$(CC) main.o library.o -o output

static: 
	$(CC) -c library.c
	ar cr liblibrary.a liblibrary.o
	$(CC) main.c -o main -L . -l liblibrary

main.o: main.c
	$(CC) $(CFLAGS) main.c

library.o: library.c library.h
	$(CC) $(CFLAGS) library.c library.h

clean:
	rm *.o output *.gch
