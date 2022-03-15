CC=gcc
CFLAGS=-c -Wall
LIB=filestatslib

lib_static: $(LIB).c
	$(CC) $(CFLAGS) $(LIB).c
	ar cr $(LIB).a $(LIB).o

lib_shared: $(LIB).c
	$(CC) $(CFLAGS) -fPIC $(LIB).c
	$(CC) -shared -o $(LIB).so $(LIB).o

lib_all:
	make lib_static
	make lib_shared

clean:
	rm *.o *.gch *.a *.so
