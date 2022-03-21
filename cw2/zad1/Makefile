CC=gcc
CFLAGS=-Wall -O3
TEST_DIR_PATH=../test/
REPORT_FILE=pomiar_zad_1.txt

all:
	make output MAIN_PATH=./ MAIN=main_lib OUTPUT=output_lib
	make output MAIN_PATH=./ MAIN=main_sys OUTPUT=output_sys

test:
	make all
	make generate_test_files
	rm -f $(REPORT_FILE)
	for i in 1 2 3 ; do \
		make test_sub OUTPUT=output_lib TEST_FILE=test$$i.txt RESULT_FILE=result$$i.txt ; \
		make test_sub OUTPUT=output_sys TEST_FILE=test$$i.txt RESULT_FILE=result$$i.txt ; \
	done ; \

output: $(MAIN_PATH)$(MAIN).c
	$(CC) $(CFLAGS) $(MAIN_PATH)$(MAIN).c -o $(OUTPUT)

generate_test_files: $(TEST_DIR_PATH)generate_test_files.sh
	$(TEST_DIR_PATH)generate_test_files.sh $(TEST_DIR_PATH)sample1.txt test1.txt 500
	$(TEST_DIR_PATH)generate_test_files.sh $(TEST_DIR_PATH)sample2.txt test2.txt 1500
	$(TEST_DIR_PATH)generate_test_files.sh $(TEST_DIR_PATH)sample3.txt test3.txt 4000

test_sub:
	$(eval FILE_SIZE=$(shell find $(TEST_FILE) -printf "%k"))
	echo "Run $(OUTPUT) with $(TEST_FILE) (size: $(FILE_SIZE)[KB]) as the source file" >> $(REPORT_FILE)
	{ time -p ./$(OUTPUT) $(TEST_FILE) $(RESULT_FILE); } 2>> $(REPORT_FILE)
	echo "" >> $(REPORT_FILE)

clean:
	rm -f *.o *.gch *.a output* test*.txt result*.txt
