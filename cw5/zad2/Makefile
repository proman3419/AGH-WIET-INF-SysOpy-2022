CC=gcc
CFLAGS=-Wall -O3

all:
	make output MAIN_PATH=./ MAIN=main OUTPUT=output

test:
	make all

	@echo "##################################################"
	@echo "Sending a mail"
	@echo "##################################################"
	./output "przemek@portable-basement" "Subject" "Content"
	@echo

	@echo "##################################################"
	@echo "Listing mails, ordering by date"
	@echo "##################################################"
	./output date
	@echo

	@echo "##################################################"
	@echo "Listing mails, ordering by author"
	@echo "##################################################"
	./output author

output: $(MAIN_PATH)$(MAIN).c
	$(CC) $(CFLAGS) $(MAIN_PATH)$(MAIN).c -o $(OUTPUT)

clean:
	rm -f *.o output*
