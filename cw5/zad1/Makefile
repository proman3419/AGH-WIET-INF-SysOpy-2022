CC=gcc
CFLAGS=-Wall -O3

all:
	make output MAIN_PATH=./ MAIN=main OUTPUT=output

test:
	make all
	make test_sub TEST_FILE=test.txt

output: $(MAIN_PATH)$(MAIN).c
	$(CC) $(CFLAGS) $(MAIN_PATH)$(MAIN).c -o $(OUTPUT)

test_sub:
	@echo "##################################################"
	@echo "Testing $(TEST_FILE)"
	@echo "##################################################"
	@./output $(TEST_FILE)

clean:
	rm -f *.o output*
