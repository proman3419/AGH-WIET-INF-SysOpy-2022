CC=gcc
CFLAGS=-Wall -O3

all:
	make output_client
	make output_server

test:
	make all
	echo "Implement"

run_client:
	make output_client
	./output_client

run_server:
	make output_server
	./output_server

output: $(MAIN_PATH)$(MAIN).c
	make lib_static LIB_PATH=./ LIB=common
	$(CC) $(CFLAGS) $(MAIN_PATH)$(MAIN).c -o $(OUTPUT) -L . common.a

output_client: client.c
	make output MAIN_PATH=./ MAIN=client OUTPUT=output_client

output_server: server.c
	make output MAIN_PATH=./ MAIN=server OUTPUT=output_server

lib_static: $(LIB_PATH)$(LIB).c
	$(CC) $(CFLAGS) -c $(LIB_PATH)$(LIB).c
	ar cr $(LIB).a $(LIB).o

clean:
	rm -f *.o *.gch *.a *.so output*
