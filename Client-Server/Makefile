CC = gcc
CFLAG = -Wall -Wextra -g -o

all: 
	$(CC) sender.c $(CFLAG) sender_client.0
	$(CC) measure.c $(CFLAG) measure_server.o

git:
	git add -A
	git commit -m "$m"
	git push

test:
	./test.sh

clean:
	rm -f *.o output/1mb.txt