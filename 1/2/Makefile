CC=gcc
CFLAGS=-c -Wall
LIB=filestatslib
MAIN=main
OUTPUT=output
STATIC_LIB=$(addsuffix -static, $(LIB)).a
SHARED_LIB=$(addsuffix -shared, $(LIB)).so
STATIC_OUTPUT=$(addsuffix -static, $(OUTPUT))
SHARED_OUTPUT=$(addsuffix -shared, $(OUTPUT))

run_all:
	make run_static
	make run_shared

static: $(LIB).c
	$(CC) $(CFLAGS) $(LIB).c
	ar cr $(STATIC_LIB) $(LIB).o

shared: $(LIB).c
	$(CC) $(CFLAGS) -fPIC $(LIB).c
	$(CC) -shared -o $(SHARED_LIB) $(LIB).o

output_static: $(MAIN).c
	make static
	$(CC) $(MAIN).c -o $(STATIC_OUTPUT) -L . $(STATIC_LIB)

output_shared: $(MAIN).c
	make shared
	$(CC) $(MAIN).c -o $(SHARED_OUTPUT) -L . $(SHARED_LIB)

run_static:
	make output_static
	LD_LIBRARY_PATH=.
	./$(STATIC_OUTPUT)

run_shared:
	make output_shared
	LD_LIBRARY_PATH=.
	./$(SHARED_OUTPUT)

clean:
	rm *.o *.gch *.a *.so $(OUTPUT)* *.tmp
