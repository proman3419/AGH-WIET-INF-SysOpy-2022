CC=gcc
CFLAGS=-Wall -O3

all:
	make output MAIN_PATH=./ MAIN=sender OUTPUT=output_sender
	make output MAIN_PATH=./ MAIN=catcher OUTPUT=output_catcher

output: $(MAIN_PATH)$(MAIN).c
	$(CC) $(CFLAGS) $(MAIN_PATH)$(MAIN).c -o $(OUTPUT)

clean:
	rm -f *.o output*
