CC=gcc
CFLAGS=-Wall -O3

all:
	make output MAIN_PATH=./ MAIN=main_fork OUTPUT=output_fork
	make output MAIN_PATH=./ MAIN=main_exec OUTPUT=output_exec

test:
	make all
	make -s test_all_actions OUTPUT=output_fork
	make -s test_all_actions OUTPUT=output_exec

output: $(MAIN_PATH)$(MAIN).c
	$(CC) $(CFLAGS) $(MAIN_PATH)$(MAIN).c -o $(OUTPUT)

test_all_actions: $(OUTPUT)
	make -s test_sub ACTION=ignore
	make -s test_sub ACTION=handler
	make -s test_sub ACTION=mask
	make -s test_sub ACTION=pending

test_sub: $(OUTPUT)
	echo "##################################################"
	echo "Testing $(OUTPUT), the action is '$(ACTION)'"
	echo "##################################################"
	./$(OUTPUT) $(ACTION)
	echo

clean:
	rm -f *.o output*
