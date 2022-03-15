CC=gcc
CFLAGS=-Wall
MAIN=main
OUTPUT=output
OUTPUT_STATIC=$(addsuffix _static, $(OUTPUT))
OUTPUT_SHARED=$(addsuffix _shared, $(OUTPUT))
REPORT_FILE="report3a.txt"
REPORT_LINES_COUNT=11

lib_static: $(LIB).c
	$(CC) $(CFLAGS) -c $(LIB).c
	ar cr $(LIB).a $(LIB).o

lib_shared: $(LIB).c
	$(CC) $(CFLAGS) -c -fPIC $(LIB).c
	$(CC) -shared -o $(LIB).so $(LIB).o

lib_dynamic: $(LIB).c
	$(CC) $(CFLAGS) -c $(LIB).c
	$(CC) -shared -o $(LIB).dll -Wl,--out-implib,libtstdll.a test_dll.o
	lib_shared
	$(CC) $(CFLAGS) -c -fPIC $(LIB).c
	$(CC) -shared -o $(LIB).so .o

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

generate_test_files:
	./generate_test_files.sh $(TEST_SIZE) $(FILE_SIZE) $(FILES_COUNT)

test_general: $(OUTPUT_TEST)
	make generate_test_files 

	echo "1 $(FILE_SIZE)[MB] file" >> $(REPORT_FILE)
	LD_LIBRARY_PATH=. ./$(OUTPUT_TEST) create_table 1 wc_files $(TEST_SIZE)0.txt | tail -n $(REPORT_LINES_COUNT) >> $(REPORT_FILE)

	echo "5 $(FILE_SIZE)[MB] files" >> $(REPORT_FILE)
	LD_LIBRARY_PATH=. ./$(OUTPUT_TEST) create_table 5 wc_files $(TEST_SIZE)0.txt $(TEST_SIZE)1.txt $(TEST_SIZE)2.txt $(TEST_SIZE)3.txt $(TEST_SIZE)4.txt | tail -n $(REPORT_LINES_COUNT) >> $(REPORT_FILE)

	echo "10 $(FILE_SIZE)[MB] files" >> $(REPORT_FILE)
	LD_LIBRARY_PATH=. ./$(OUTPUT_TEST) create_table 10 wc_files $(TEST_SIZE)0.txt $(TEST_SIZE)1.txt $(TEST_SIZE)2.txt $(TEST_SIZE)3.txt $(TEST_SIZE)4.txt $(TEST_SIZE)5.txt $(TEST_SIZE)6.txt $(TEST_SIZE)7.txt $(TEST_SIZE)8.txt $(TEST_SIZE)9.txt | tail -n $(REPORT_LINES_COUNT) >> $(REPORT_FILE)

test_allocation: $(OUTPUT_TEST)
	echo "Create, remove 1000 1[kB] blocks, repeat 1000 times" >> $(REPORT_FILE)
	LD_LIBRARY_PATH=. ./$(OUTPUT_TEST) create_remove_blocks 1000 1000 1000 | tail -n $(REPORT_LINES_COUNT) >> $(REPORT_FILE)
	echo "Create, remove 100 10[kB] blocks, repeat 1000 times" >> $(REPORT_FILE)
	LD_LIBRARY_PATH=. ./$(OUTPUT_TEST) create_remove_blocks 100 10000 1000 | tail -n $(REPORT_LINES_COUNT) >> $(REPORT_FILE)
	echo "Create, remove 100 1[MB] blocks, repeat 1000 times" >> $(REPORT_FILE)
	LD_LIBRARY_PATH=. ./$(OUTPUT_TEST) create_remove_blocks 100 1000000 1000 | tail -n $(REPORT_LINES_COUNT) >> $(REPORT_FILE)

test: $(OUTPUT_TEST)
	touch $(REPORT_FILE)
	make test_general TEST_SIZE="small" FILES_COUNT=10 FILE_SIZE=1
	make test_general TEST_SIZE="medium" FILES_COUNT=10 FILE_SIZE=10
	make test_general TEST_SIZE="large" FILES_COUNT=10 FILE_SIZE=100
	make test_allocation 
	less $(REPORT_FILE)

test_static:
	make output_static
	make test OUTPUT_TEST=$(OUTPUT_STATIC)

test_shared:
	make output_shared
	make test OUTPUT_TEST=$(OUTPUT_SHARED)

test_all:
	make test_static
	make test_shared

clean:
	rm *.o *.gch *.a *.so $(OUTPUT)* *.tmp small*.txt medium*.txt large*.txt
