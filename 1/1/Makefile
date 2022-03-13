CC=gcc
CFLAGS=-c -Wall
LIB_NAME=filestatslib
STATIC_LIB_FILE=$(addsuffix -static, $(LIB_NAME)).a
SHARED_LIB_FILE=$(addsuffix -shared, $(LIB_NAME)).so

all: static shared

static:
	$(CC) $(CFLAGS) $(LIB_NAME).c
	ar cr $(STATIC_LIB_FILE) $(LIB_NAME).o

shared:
	$(CC) $(CFLAGS) -fPIC $(LIB_NAME).c
	$(CC) -shared -fPIC -o $(SHARED_LIB_FILE) $(LIB_NAME).o

clean:
	rm *.o *.gch *.a *.so
