CC=gcc
CFLAGS=-c -Wall
LIB=filestatslib
MAIN=main
OUTPUT=output
STATIC_LIB=$(addsuffix -static, $(LIB)).a
SHARED_LIB=$(addsuffix -shared, $(LIB)).so
STATIC_OUTPUT=$(addsuffix -static, $(OUTPUT))
SHARED_OUTPUT=$(addsuffix -shared, $(OUTPUT))

run_all:
	make run_static
	make run_shared

static: $(LIB).c
	$(CC) $(CFLAGS) $(LIB).c
	ar cr $(STATIC_LIB) $(LIB).o

shared: $(LIB).c
	$(CC) $(CFLAGS) -fPIC $(LIB).c
	$(CC) -shared -o $(SHARED_LIB) $(LIB).o

output_static: $(MAIN).c
	make static
	$(CC) $(MAIN).c -o $(STATIC_OUTPUT) -L . $(STATIC_LIB)

output_shared: $(MAIN).c
	make shared
	$(CC) $(MAIN).c -o $(SHARED_OUTPUT) -L . $(SHARED_LIB)

run_static:
	make output_static
	LD_LIBRARY_PATH=.
	./$(STATIC_OUTPUT)

run_shared:
	make output_shared
	LD_LIBRARY_PATH=.
	./$(SHARED_OUTPUT)

generate_test_files: generate_test_files.py
	./generate_test_files.py 10 1000 small
	./generate_test_files.py 10 100000 medium
	./generate_test_files.py 10 1000000 large

test: $(TEST_OUTPUT)
	make generate_test_files
	make output_static
	./$(TEST_OUTPUT) create_table 1 wc_files small0.txt
	./$(TEST_OUTPUT) create_table 5 wc_files small0.txt small1.txt small2.txt small3.txt small4.txt
	./$(TEST_OUTPUT) create_table 10 wc_files small0.txt small1.txt small2.txt small3.txt small4.txt small5.txt small6.txt small7.txt small8.txt small9.txt
	./$(TEST_OUTPUT) create_table 1 wc_files medium0.txt
	./$(TEST_OUTPUT) create_table 5 wc_files medium0.txt medium1.txt medium2.txt medium3.txt medium4.txt
	./$(TEST_OUTPUT) create_table 10 wc_files medium0.txt medium1.txt medium2.txt medium3.txt medium4.txt medium5.txt medium6.txt medium7.txt medium8.txt medium9.txt
	./$(TEST_OUTPUT) create_table 1 wc_files large0.txt
	./$(TEST_OUTPUT) create_table 5 wc_files large0.txt large1.txt large2.txt large3.txt large4.txt
	./$(TEST_OUTPUT) create_table 10 wc_files large0.txt large1.txt large2.txt large3.txt large4.txt large5.txt large6.txt large7.txt large8.txt large9.txt

test_static:
	make test TEST_OUTPUT=$(STATIC_OUTPUT)

clean:
	rm *.o *.gch *.a *.so $(OUTPUT)* *.tmp *.txt
