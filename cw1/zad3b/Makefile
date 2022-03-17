CC=gcc
CFLAGS=-Wall
MAIN_PATH=../zad2/
MAIN=main
OUTPUT=output
OUTPUT_STATIC=$(addsuffix _static, $(OUTPUT))
OUTPUT_SHARED=$(addsuffix _shared, $(OUTPUT))
OUTPUT_DYNAMIC=$(addsuffix _dynamic, $(OUTPUT))
FILESTATSLIB_PATH=../zad1/
TIMEMEASLIB_PATH=../zad2/
GENERATE_TEST_FILES_PATH=../zad2/
REPORT_FILE=results3b.txt
REPORT_LINES_COUNT=11

all:
	echo "make output_<static/shared/dynamic>_<optimalization flags>"

test:
	make generate_test_files
	rm -f $(REPORT_FILE)
	make test_static OPT_FLAGS=-O0
	make test_static OPT_FLAGS=-O
	make test_static OPT_FLAGS=-O2
	make test_static OPT_FLAGS=-O3

lib_static: $(LIB_PATH)$(LIB).c
	$(CC) $(CFLAGS) $(OPT_FLAGS) -c $(LIB_PATH)$(LIB).c
	ar cr $(LIB).a $(LIB).o

lib_shared: $(LIB_PATH)$(LIB).c
	$(CC) $(CFLAGS) $(OPT_FLAGS) -c -fPIC $(LIB_PATH)$(LIB).c
	$(CC) -shared -o $(LIB).so $(LIB).o

output_static: $(MAIN_PATH)$(MAIN).c
	make lib_static LIB_PATH=$(FILESTATSLIB_PATH) LIB=filestatslib
	make lib_static LIB_PATH=$(TIMEMEASLIB_PATH) LIB=timemeaslib
	$(CC) $(CFLAGS) $(OPT_FLAGS) $(MAIN_PATH)$(MAIN).c -o $(OUTPUT_STATIC) -L . filestatslib.a timemeaslib.a

output_shared: $(MAIN_PATH)$(MAIN).c
	make lib_shared LIB_PATH=$(FILESTATSLIB_PATH) LIB=filestatslib
	make lib_shared LIB_PATH=$(TIMEMEASLIB_PATH) LIB=timemeaslib
	$(CC) $(CFLAGS) $(OPT_FLAGS) $(MAIN_PATH)$(MAIN).c -o $(OUTPUT_SHARED) -L . filestatslib.so timemeaslib.so

output_dynamic:
	make lib_shared LIB_PATH=$(FILESTATSLIB_PATH) LIB=filestatslib
	make lib_shared LIB_PATH=$(TIMEMEASLIB_PATH) LIB=timemeaslib
	$(CC) $(CFLAGS) $(MAIN_PATH)$(MAIN).c $(FILESTATSLIB_PATH)filestatslib.c $(TIMEMEASLIB_PATH)timemeaslib.c -ldl -o $(OUTPUT_DYNAMIC) -I . -D DLL

output_static_O:
	make output_static OPT_FLAGS=-O

output_static_O1:
	make output_static OPT_FLAGS=-O1

output_static_O2:
	make output_static OPT_FLAGS=-O2

output_static_O3:
	make output_static OPT_FLAGS=-O3	

output_static_O0:
	make output_static OPT_FLAGS=-O0

output_static_Os:
	make output_static OPT_FLAGS=-Os

output_shared_O:
	make output_shared OPT_FLAGS=-O

output_shared_O1:
	make output_shared OPT_FLAGS=-O1

output_shared_O2:
	make output_shared OPT_FLAGS=-O2

output_shared_O3:
	make output_shared OPT_FLAGS=-O3

output_shared_O0:
	make output_shared OPT_FLAGS=-O0

output_dynamic_O:
	make output_dynamic OPT_FLAGS=-O

output_dynamic_O1:
	make output_dynamic OPT_FLAGS=-O1

output_dynamic_O2:
	make output_dynamic OPT_FLAGS=-O2

output_dynamic_O3:
	make output_dynamic OPT_FLAGS=-O3

output_dynamic_O0:
	make output_dynamic OPT_FLAGS=-O0

output_dynamic_Os:
	make output_dynamic OPT_FLAGS=-Os

run_static:
	make output_static
	./$(OUTPUT_STATIC)

run_shared:
	make output_shared
	LD_LIBRARY_PATH=. ./$(OUTPUT_SHARED)

run_dynamic:
	make output_dynamic
	LD_LIBRARY_PATH=. ./$(OUTPUT_DYNAMIC)

run_all:
	make run_static
	make run_shared
	make run_dynamic

generate_test_files:
	$(GENERATE_TEST_FILES_PATH)generate_test_files.sh small 1 10
	$(GENERATE_TEST_FILES_PATH)generate_test_files.sh medium 10 10
	$(GENERATE_TEST_FILES_PATH)generate_test_files.sh large 100 10

test_general: $(OUTPUT_TEST)
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
	echo "############################################" >> $(REPORT_FILE)
	echo "Testing $(OUTPUT_TEST) with $(OPT_FLAGS) flags" >> $(REPORT_FILE)
	echo "############################################" >> $(REPORT_FILE)
	echo "" >> $(REPORT_FILE)
	make test_general TEST_SIZE="small" FILE_SIZE=1 FILES_COUNT=10
	make test_general TEST_SIZE="medium" FILE_SIZE=10 FILES_COUNT=10
	make test_general TEST_SIZE="large" FILE_SIZE=100 FILES_COUNT=10
	make test_allocation 

test_static:
	make output_static
	make test_sub OUTPUT_TEST=$(OUTPUT_STATIC)

clean:
	rm -f *.o *.gch *.a *.so $(OUTPUT)* *.tmp small*.txt medium*.txt large*.txt
