CC=gcc
CFLAGS=-Wall -O3

all:
	touch oven
	touch table
	make output MAIN_PATH=./ MAIN=chef OUTPUT=output_chef
	make output MAIN_PATH=./ MAIN=carrier OUTPUT=output_carrier
	make output MAIN_PATH=./ MAIN=main OUTPUT=output

test:
	make clean
	make all
	./output 5 5

output: $(MAIN_PATH)$(MAIN).c
	$(CC) $(CFLAGS) $(MAIN_PATH)$(MAIN).c -o $(OUTPUT)

clean:
	rm -f *.o output* oven table
