CC=gcc
CFLAGS=-Wall -O3
TEST_DIR_PATH=../test/
REPORT_FILE=report3.txt

all:
	make lib LIB_PATH=./ LIB=filetypeocclib
	make output MAIN_PATH=./ MAIN=main_dirstat OUTPUT=output_dirstat

test:
	make all
	rm -f $(REPORT_FILE)
	make test_sub OUTPUT=output_dirstat TEST_DIR=$(TEST_DIR_PATH)test3
	make test_sub OUTPUT=output_dirstat TEST_DIR=/tmp/

output: $(MAIN_PATH)$(MAIN).c
	$(CC) $(CFLAGS) $(MAIN_PATH)$(MAIN).c -o $(OUTPUT) -L . filetypeocclib.a

lib: $(LIB_PATH)$(LIB).c
	$(CC) $(CFLAGS) -c $(LIB_PATH)$(LIB).c
	ar cr $(LIB).a $(LIB).o

test_sub:
	echo "Run $(OUTPUT), explore $(TEST_DIR)" >> $(REPORT_FILE)
	./$(OUTPUT) $(TEST_DIR) >> $(REPORT_FILE)
	echo "" >> $(REPORT_FILE)

clean:
	rm -f *.o *.gch *.a output* test*.txt result*.txt
