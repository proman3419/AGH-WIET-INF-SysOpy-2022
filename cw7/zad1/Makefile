CC=gcc
CFLAGS=-Wall -O3

all:
	touch OVEN
	touch TABLE
	make output MAIN_PATH=./ MAIN=chef OUTPUT=output_chef
	make output MAIN_PATH=./ MAIN=carrier OUTPUT=output_carrier
	make output MAIN_PATH=./ MAIN=main OUTPUT=output

test_small:
	make all
	./output 3 3

test_big:
	make all
	./output 7 7

output: $(MAIN_PATH)$(MAIN).c
	make lib_static LIB_PATH=./ LIB=common
	$(CC) $(CFLAGS) $(MAIN_PATH)$(MAIN).c -o $(OUTPUT) -L . common.a

lib_static: $(LIB_PATH)$(LIB).c
	$(CC) $(CFLAGS) -c $(LIB_PATH)$(LIB).c
	ar cr $(LIB).a $(LIB).o

clean:
	rm -f *.o output* *.a OVEN TABLE
