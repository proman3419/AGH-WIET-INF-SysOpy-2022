CC=gcc
CFLAGS=-Wall -O3
REPORT_FILE=report2.txt
REPORT_LINES_COUNT=7
OUTPUT=output

all:
	make output MAIN_PATH=./ MAIN=main

test:
	make all
	rm -f $(REPORT_FILE)
	make test_sub RECTANGLE_WIDTH=0.1 PROCESSES_COUNT=10000
	make test_sub RECTANGLE_WIDTH=0.01 PROCESSES_COUNT=10000
	make test_sub RECTANGLE_WIDTH=0.01 PROCESSES_COUNT=25000
	make test_sub RECTANGLE_WIDTH=0.001 PROCESSES_COUNT=25000
	make test_sub RECTANGLE_WIDTH=0.0001 PROCESSES_COUNT=30000

output: $(MAIN_PATH)$(MAIN).c
	make lib LIB_PATH=$(TIMEMEASLIB_PATH) LIB=timemeaslib
	$(CC) $(CFLAGS) $(MAIN_PATH)$(MAIN).c -o $(OUTPUT) -L . timemeaslib.a

lib: $(LIB_PATH)$(LIB).c
	$(CC) $(CFLAGS) -c $(LIB_PATH)$(LIB).c
	ar cr $(LIB).a $(LIB).o

test_sub:
	echo "##################################################" >> $(REPORT_FILE)
	echo "rectangle width = $(RECTANGLE_WIDTH), processes count = $(PROCESSES_COUNT)" >> $(REPORT_FILE)
	echo "##################################################" >> $(REPORT_FILE)
	echo "" >> $(REPORT_FILE)
	./$(OUTPUT) $(RECTANGLE_WIDTH) $(PROCESSES_COUNT) | tail -n $(REPORT_LINES_COUNT) >>  $(REPORT_FILE)

clean:
	rm -f *.o output* w*.txt
