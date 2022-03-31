CC=gcc
CFLAGS=-Wall -O3
OUTPUT=output

all:
	make output MAIN_PATH=./ MAIN=main

test:
	make all
	tar -xf test.tar.gz 
	./$(OUTPUT) test hello 1
	./$(OUTPUT) test test 1
	./$(OUTPUT) test pingwin 1
	./$(OUTPUT) test pingwin 2
	rm -rf ./test

output: $(MAIN_PATH)$(MAIN).c
	$(CC) $(CFLAGS) $(MAIN_PATH)$(MAIN).c -o $(OUTPUT)

clean:
	rm -f *.o output*
