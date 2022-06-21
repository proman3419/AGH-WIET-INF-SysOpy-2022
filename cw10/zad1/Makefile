CC=gcc
CFLAGS=-Wall -O3

compile:
	make output MAIN_PATH=./ MAIN=server OUTPUT=output_server
	make output MAIN_PATH=./ MAIN=client OUTPUT=output_client

output: $(MAIN_PATH)$(MAIN).c
	$(CC) $(CFLAGS) $(MAIN_PATH)$(MAIN).c -o $(OUTPUT) -lpthread

run_server:
	./output_server 9999 sock

run_client_1:
	./output_client "PLAYER1" unix sock

run_client_2:
	./output_client "PLAYER2" unix sock

run_client_3:
	./output_client "PLAYER3" unix sock

run_client_4:
	./output_client "PLAYER4" unix sock

clean:
	rm -f output* sock
