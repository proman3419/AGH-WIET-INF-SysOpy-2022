CC=gcc
CFLAGS=-Wall -O3

all:
	make output MAIN_PATH=./ MAIN=main_fork OUTPUT=output_fork

test:
	make all
	make -s test_all_actions OUTPUT_TEST=output_fork

output: $(MAIN_PATH)$(MAIN).c
	$(CC) $(CFLAGS) $(MAIN_PATH)$(MAIN).c -o $(OUTPUT)

test_all_actions: $(OUTPUT_TEST)
	make -s test_sub ACTION=ignore
	make -s test_sub ACTION=handler
	make -s test_sub ACTION=mask
	make -s test_sub ACTION=pending

test_sub: $(OUTPUT_TEST)
	echo "##################################################"
	echo "Testing $(OUTPUT_TEST), the action is '$(ACTION)'"
	echo "##################################################"
	./$(OUTPUT_TEST) $(ACTION)
	echo

clean:
	rm -f *.o output*
