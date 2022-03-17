CC=gcc
CFLAGS=-Wall
MAIN_PATH=./
MAIN=main
OUTPUT=output
OUTPUT_STATIC=$(addsuffix _static, $(OUTPUT))
FILESTATSLIB_PATH=../zad1/
TIMEMEASLIB_PATH=./
GENERATE_TEST_FILES_PATH=./
REPORT_FILE=report2.txt
REPORT_LINES_COUNT=11

all:
	make output_static

test:
	make output_static
	make test_sub OUTPUT_TEST=$(OUTPUT_STATIC)

lib_static: $(LIB_PATH)$(LIB).c
	$(CC) $(CFLAGS) -c $(LIB_PATH)$(LIB).c
	ar cr $(LIB).a $(LIB).o

output_static: $(MAIN_PATH)$(MAIN).c
	make lib_static LIB_PATH=$(FILESTATSLIB_PATH) LIB=filestatslib
	make lib_static LIB_PATH=$(TIMEMEASLIB_PATH) LIB=timemeaslib
	$(CC) $(CFLAGS) $(MAIN_PATH)$(MAIN).c -o $(OUTPUT_STATIC) -L . filestatslib.a timemeaslib.a

run_static:
	make output_static
	./$(OUTPUT_STATIC)

generate_test_files:
	$(GENERATE_TEST_FILES_PATH)generate_test_files.sh $(TEST_SIZE) $(FILE_SIZE) $(FILES_COUNT)

test_general: $(OUTPUT_TEST)
	make generate_test_files 

	echo "1 $(FILE_SIZE)[MB] file" >> $(REPORT_FILE)
	LD_LIBRARY_PATH=. ./$(OUTPUT_TEST) create_table 1 wc_files $(TEST_SIZE)0.txt | tail -n $(REPORT_LINES_COUNT) >> $(REPORT_FILE)

	echo "5 $(FILE_SIZE)[MB] files" >> $(REPORT_FILE)
	LD_LIBRARY_PATH=. ./$(OUTPUT_TEST) create_table 5 wc_files $(TEST_SIZE)0.txt $(TEST_SIZE)1.txt $(TEST_SIZE)2.txt $(TEST_SIZE)3.txt $(TEST_SIZE)4.txt | tail -n $(REPORT_LINES_COUNT) >> $(REPORT_FILE)

	echo "10 $(FILE_SIZE)[MB] files" >> $(REPORT_FILE)
	LD_LIBRARY_PATH=. ./$(OUTPUT_TEST) create_table 10 wc_files $(TEST_SIZE)0.txt $(TEST_SIZE)1.txt $(TEST_SIZE)2.txt $(TEST_SIZE)3.txt $(TEST_SIZE)4.txt $(TEST_SIZE)5.txt $(TEST_SIZE)6.txt $(TEST_SIZE)7.txt $(TEST_SIZE)8.txt $(TEST_SIZE)9.txt | tail -n $(REPORT_LINES_COUNT) >> $(REPORT_FILE)

test_allocation: $(OUTPUT_TEST)
	echo "Create, remove 1000 1[kB] blocks, repeat 100 times" >> $(REPORT_FILE)
	LD_LIBRARY_PATH=. ./$(OUTPUT_TEST) create_remove_blocks 1000 1000 100 | tail -n $(REPORT_LINES_COUNT) >> $(REPORT_FILE)
	echo "Create, remove 100 10[kB] blocks, repeat 100 times" >> $(REPORT_FILE)
	LD_LIBRARY_PATH=. ./$(OUTPUT_TEST) create_remove_blocks 100 10000 100 | tail -n $(REPORT_LINES_COUNT) >> $(REPORT_FILE)
	echo "Create, remove 100 1[MB] blocks, repeat 100 times" >> $(REPORT_FILE)
	LD_LIBRARY_PATH=. ./$(OUTPUT_TEST) create_remove_blocks 100 1000000 100 | tail -n $(REPORT_LINES_COUNT) >> $(REPORT_FILE)

test_sub: $(OUTPUT_TEST)
	rm -f $(REPORT_FILE)
	make test_general TEST_SIZE="small" FILES_COUNT=10 FILE_SIZE=1
	make test_general TEST_SIZE="medium" FILES_COUNT=10 FILE_SIZE=10
	make test_general TEST_SIZE="large" FILES_COUNT=10 FILE_SIZE=100
	make test_allocation 
	less $(REPORT_FILE)

clean:
	rm -f *.o *.gch *.a *.so $(OUTPUT)* *.tmp small*.txt medium*.txt large*.txt
