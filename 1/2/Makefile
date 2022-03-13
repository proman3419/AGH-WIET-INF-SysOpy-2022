CC=gcc
CFLAGS=-c -Wall
LIB=filestatslib
MAIN=main
OUTPUT=output
STATIC_LIB=$(addsuffix -static, $(LIB)).a
SHARED_LIB=$(addsuffix -shared, $(LIB)).so
STATIC_OUTPUT=$(addsuffix -static, $(OUTPUT))
SHARED_OUTPUT=$(addsuffix -shared, $(OUTPUT))

all: 
	make static_output
	make shared_output

static: $(LIB).c
	$(CC) $(CFLAGS) $(LIB).c
	ar cr $(STATIC_LIB) $(LIB).o

shared: $(LIB).c
	$(CC) $(CFLAGS) -fPIC $(LIB).c
	$(CC) -shared -fPIC -o $(SHARED_LIB) $(LIB).o

static_output: $(MAIN).c
	make static
	$(CC) $(MAIN).c -o $(STATIC_OUTPUT) -L . $(STATIC_LIB)

shared_output: $(MAIN).c
	make shared
	$(CC) $(MAIN).c -o $(SHARED_OUTPUT) -L . $(SHARED_LIB)

clean:
	rm *.o *.gch *.a *.so $(OUTPUT)*
