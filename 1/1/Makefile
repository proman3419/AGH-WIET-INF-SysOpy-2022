CC=gcc
CFLAGS=-c -Wall
LIB=filestatslib

lib_all:
	make lib_static
	make lib_shared

lib_static: $(LIB).c
	$(CC) $(CFLAGS) $(LIB).c
	ar cr $(LIB).a $(LIB).o

lib_shared: $(LIB).c
	$(CC) $(CFLAGS) -fPIC $(LIB).c
	$(CC) -shared -o $(LIB).so $(LIB).o

clean:
	rm *.o *.gch *.a *.so
