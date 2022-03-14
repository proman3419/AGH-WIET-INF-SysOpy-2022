CC=gcc
CFLAGS=-c -Wall
MAIN=main
OUTPUT=output
OUTPUT_STATIC=$(addsuffix _static, $(OUTPUT))
OUTPUT_SHARED=$(addsuffix _shared, $(OUTPUT))

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
	./generate_test_files.py 10 1000000 large

test: $(OUTPUT_TEST)
	make generate_test_files
	LD_LIBRARY_PATH=. ./$(OUTPUT_TEST) create_table 1 wc_files small0.txt
	LD_LIBRARY_PATH=. ./$(OUTPUT_TEST) create_table 5 wc_files small0.txt small1.txt small2.txt small3.txt small4.txt
	LD_LIBRARY_PATH=. ./$(OUTPUT_TEST) create_table 10 wc_files small0.txt small1.txt small2.txt small3.txt small4.txt small5.txt small6.txt small7.txt small8.txt small9.txt
	LD_LIBRARY_PATH=. ./$(OUTPUT_TEST) create_table 1 wc_files medium0.txt
	LD_LIBRARY_PATH=. ./$(OUTPUT_TEST) create_table 5 wc_files medium0.txt medium1.txt medium2.txt medium3.txt medium4.txt
	LD_LIBRARY_PATH=. ./$(OUTPUT_TEST) create_table 10 wc_files medium0.txt medium1.txt medium2.txt medium3.txt medium4.txt medium5.txt medium6.txt medium7.txt medium8.txt medium9.txt
	LD_LIBRARY_PATH=. ./$(OUTPUT_TEST) create_table 1 wc_files large0.txt
	LD_LIBRARY_PATH=. ./$(OUTPUT_TEST) create_table 5 wc_files large0.txt large1.txt large2.txt large3.txt large4.txt
	LD_LIBRARY_PATH=. ./$(OUTPUT_TEST) create_table 10 wc_files large0.txt large1.txt large2.txt large3.txt large4.txt large5.txt large6.txt large7.txt large8.txt large9.txt

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
	rm *.o *.gch *.a *.so $(OUTPUT)* *.tmp *.txt