CC=gcc
CFLAGS=-c -Wall
LIB=filestatslib
STATIC_LIB=$(addsuffix -static, $(LIB)).a
SHARED_LIB=$(addsuffix -shared, $(LIB)).so

all: 
	make static
	make shared

static: $(LIB).c
	$(CC) $(CFLAGS) $(LIB).c
	ar cr $(STATIC_LIB) $(LIB).o

shared: $(LIB).c
	$(CC) $(CFLAGS) -fPIC $(LIB).c
	$(CC) -shared -fPIC -o $(SHARED_LIB) $(LIB).o

clean:
	rm *.o *.gch *.a *.so
