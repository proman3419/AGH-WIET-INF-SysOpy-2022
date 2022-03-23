CC=gcc
CFLAGS=-Wall -O3
TEST_DIR_PATH=../test/
REPORT_FILE=report3.txt
REPORT_LINES_COUNT=12

all:
	make lib LIB_PATH=./ LIB=filetypeocclib
	make output MAIN_PATH=./ MAIN=main_dirstat OUTPUT=output_dirstat
	make output MAIN_PATH=./ MAIN=main_nftw OUTPUT=output_nftw

test:
	make all
	rm -f $(REPORT_FILE)
	sudo tar -xf $(TEST_DIR_PATH)test3.tar.gz
	make test_sub OUTPUT_TEST=output_dirstat TEST_DIR=test3
	make test_sub OUTPUT_TEST=output_nftw TEST_DIR=test3
	rm -rf ./test3
	make test_sub OUTPUT_TEST=output_dirstat TEST_DIR=/tmp/
	make test_sub OUTPUT_TEST=output_nftw TEST_DIR=/tmp/

output: $(MAIN_PATH)$(MAIN).c
	$(CC) $(CFLAGS) $(MAIN_PATH)$(MAIN).c -o $(OUTPUT) -L . filetypeocclib.a

lib: $(LIB_PATH)$(LIB).c
	$(CC) $(CFLAGS) -c $(LIB_PATH)$(LIB).c
	ar cr $(LIB).a $(LIB).o

test_sub:
	echo "############################################" >> $(REPORT_FILE)
	echo "Testing $(OUTPUT_TEST), exploring $(TEST_DIR)" >> $(REPORT_FILE)
	echo "############################################" >> $(REPORT_FILE)
	echo "" >> $(REPORT_FILE)
	./$(OUTPUT_TEST) $(TEST_DIR) | tail -n $(REPORT_LINES_COUNT) >>  $(REPORT_FILE)

clean:
	rm -f *.o *.gch *.a output* test*.txt result*.txt
	rm -rf ./test3
