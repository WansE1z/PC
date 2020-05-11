CC=gcc -g
CFLAGS=-I.
MFLAGS= --leak-check=full --show-leak-kinds=all --track-origins=yes

client: client.c requests.c helpers.c buffer.c
	$(CC) -o client client.c requests.c helpers.c buffer.c -Wall

run: client
	./client

clean:
	rm -f *.o client

memory: clean client
	valgrind $(MFLAGS) ./client

beauty: 
	clang-format -i -style=file *.c
