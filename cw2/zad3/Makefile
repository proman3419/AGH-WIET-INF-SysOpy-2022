CC=gcc
CFLAGS=-Wall -O3
TEST_DIR_PATH=../test/

all:
	make output MAIN_PATH=./ MAIN=main_dirstat OUTPUT=output_dirstat

test:
	make all
	make generate_test_files
	rm -f $(REPORT_FILE)
	for i in 1 2 3 ; do \
		for ch in r o m a n ; do \
			make test_sub OUTPUT=output_lib TEST_CHAR=$$ch TEST_FILE=test$$i.txt RESULT_FILE=result$$i.txt ; \
			make test_sub OUTPUT=output_sys TEST_CHAR=$$ch TEST_FILE=test$$i.txt RESULT_FILE=result$$i.txt ; \
		done ; \
	done ; \

output: $(MAIN_PATH)$(MAIN).c
	$(CC) $(CFLAGS) $(MAIN_PATH)$(MAIN).c -o $(OUTPUT)

generate_test_files: $(TEST_DIR_PATH)generate_test_files.sh
	$(TEST_DIR_PATH)generate_test_files.sh $(TEST_DIR_PATH)sample1.txt test1.txt 30000
	$(TEST_DIR_PATH)generate_test_files.sh $(TEST_DIR_PATH)sample2.txt test2.txt 70000
	$(TEST_DIR_PATH)generate_test_files.sh $(TEST_DIR_PATH)sample3.txt test3.txt 150000

test_sub:
	$(eval FILE_SIZE=$(shell find $(TEST_FILE) -printf "%k"))
	echo "Run $(OUTPUT), looking for $(TEST_CHAR) in $(TEST_FILE) (size: $(FILE_SIZE)[KB])" >> $(REPORT_FILE)
	./$(OUTPUT) $(TEST_CHAR) $(TEST_FILE) 2>> $(REPORT_FILE)
	echo "" >> $(REPORT_FILE)

clean:
	rm -f *.o *.gch *.a output* test*.txt result*.txt
