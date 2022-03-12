CC=gcc
CFLAGS=-c -Wall

output: main.o library.o
	$(CC) main.o library.o -o output

main.o: main.c
	$(CC) $(CFLAGS) main.c

library.o: library.c library.h
	$(CC) $(CFLAGS) library.c library.h

clean:
	rm *.o output *.gch
