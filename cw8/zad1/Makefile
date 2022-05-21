CC=gcc
CFLAGS=-Wall -O3

all:
	make output MAIN_PATH=./ MAIN=main OUTPUT=output

test:
	make all
	mkdir -p negatives
	rm -f Times.txt

	make test_sub DIVISION_METHOD=numbers IMG_PATH=img/casablanca.ascii.pgm OUT_PATH=negatives/casablanca.ascii.pgm
	make test_sub DIVISION_METHOD=numbers IMG_PATH=img/coins.ascii.pgm OUT_PATH=negatives/coins.ascii.pgm
	make test_sub DIVISION_METHOD=numbers IMG_PATH=img/dla.ascii.pgm OUT_PATH=negatives/dla.ascii.pgm

	make test_sub DIVISION_METHOD=block IMG_PATH=img/casablanca.ascii.pgm OUT_PATH=negatives/casablanca.ascii.pgm
	make test_sub DIVISION_METHOD=block IMG_PATH=img/coins.ascii.pgm OUT_PATH=negatives/coins.ascii.pgm
	make test_sub DIVISION_METHOD=block IMG_PATH=img/dla.ascii.pgm OUT_PATH=negatives/dla.ascii.pgm

output: $(MAIN_PATH)$(MAIN).c
	$(CC) $(CFLAGS) $(MAIN_PATH)$(MAIN).c -o $(OUTPUT) -lpthread

test_sub:
	for THREADS_CNT in 1 2 4 8 16 ; do \
	    ./output $$THREADS_CNT $(DIVISION_METHOD) $(IMG_PATH) $(OUT_PATH); \
		echo ""; \
	done

clean:
	rm -f *.o output*
	rm -rf negatives
