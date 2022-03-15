CC=gcc
CFLAGS=-c -Wall
MAIN=main
OUTPUT=output
OUTPUT_STATIC=$(addsuffix _static, $(OUTPUT))
OUTPUT_SHARED=$(addsuffix _shared, $(OUTPUT))
REPORT_FILE="report2.txt"

lib_static: $(LIB).c
	$(CC) $(CFLAGS) $(LIB).c
	ar cr $(LIB).a $(LIB).o

lib_shared: $(LIB).c
	$(CC) $(CFLAGS) -fPIC $(LIB).c
	$(CC) -shared -o $(LIB).so $(LIB).o

output_static: $(MAIN).c
	make lib_static LIB=filestatslib
	make lib_static LIB=timemeaslib
	$(CC) $(MAIN).c -o $(OUTPUT_STATIC) -L . filestatslib.a timemeaslib.a

output_shared: $(MAIN).c
	make lib_shared LIB=filestatslib
	make lib_shared LIB=timemeaslib
	$(CC) $(MAIN).c -o $(OUTPUT_SHARED) -L . filestatslib.so timemeaslib.so

run_static:
	make output_static
	./$(OUTPUT_STATIC)

run_shared:
	make output_shared
	LD_LIBRARY_PATH=. ./$(OUTPUT_SHARED)

run_all:
	make run_static
	make run_shared

generate_test_files: generate_test_files.py
	./generate_test_files.py 10 1000 small
	./generate_test_files.py 10 100000 medium
# 	./generate_test_files.py 10 1000000 large

test_general: $(OUTPUT_TEST)
	echo "1 $(TEST_SIZE) file" >> $(REPORT_FILE)
	LD_LIBRARY_PATH=. ./$(OUTPUT_TEST) create_table 1 wc_files $(TEST_SIZE)0.txt | tail -n 9 >> $(REPORT_FILE)

	echo "5 $(TEST_SIZE) files" >> $(REPORT_FILE)
	LD_LIBRARY_PATH=. ./$(OUTPUT_TEST) create_table 5 wc_files $(TEST_SIZE)0.txt $(TEST_SIZE)1.txt $(TEST_SIZE)2.txt $(TEST_SIZE)3.txt $(TEST_SIZE)4.txt | tail -n 9 >> $(REPORT_FILE)

	echo "10 $(TEST_SIZE) files" >> $(REPORT_FILE)
	LD_LIBRARY_PATH=. ./$(OUTPUT_TEST) create_table 10 wc_files $(TEST_SIZE)0.txt $(TEST_SIZE)1.txt $(TEST_SIZE)2.txt $(TEST_SIZE)3.txt $(TEST_SIZE)4.txt $(TEST_SIZE)5.txt $(TEST_SIZE)6.txt $(TEST_SIZE)7.txt $(TEST_SIZE)8.txt $(TEST_SIZE)9.txt | tail -n 9 >> $(REPORT_FILE)

test_allocation: $(OUTPUT_TEST)
	echo "Allocate 10, free 10, allocate 100, free 100" >> $(REPORT_FILE)
	LD_LIBRARY_PATH=. ./$(OUTPUT_TEST) create_remove_blocks 1000 1000 1000 | tail -n 9 >> $(REPORT_FILE)

test: $(OUTPUT_TEST)
	make generate_test_files
	touch $(REPORT_FILE)
	make test_general TEST_SIZE="small"
	make test_general TEST_SIZE="medium"
# 	make test_general TEST_SIZE="large"
	make test_allocation 
	cat $(REPORT_FILE)

test_static:
	make output_static
	OUTPUT_TEST=$(OUTPUT_STATIC) make test

test_shared:
	make output_shared
	OUTPUT_TEST=$(OUTPUT_SHARED) make test

test_all:
	make test_static
	make test_shared

clean:
	rm *.o *.gch *.a *.so $(OUTPUT)* *.tmp *.txt