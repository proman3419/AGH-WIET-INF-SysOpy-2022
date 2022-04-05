CC=gcc
CFLAGS=-Wall -O3

all:
	make output MAIN_PATH=./ MAIN=main_fork OUTPUT=output_fork
	make output MAIN_PATH=./ MAIN=main_exec OUTPUT=output_exec

test:
	echo "Use 'make clean && make all && ./test.sh' (make stops execution when it receives a signal)"

output: $(MAIN_PATH)$(MAIN).c
	$(CC) $(CFLAGS) $(MAIN_PATH)$(MAIN).c -o $(OUTPUT)

clean:
	rm -f *.o output*
