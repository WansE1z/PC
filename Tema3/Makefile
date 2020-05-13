CC=gcc -g
CFLAGS=-I.
MFLAGS= --leak-check=full --show-leak-kinds=all --track-origins=yes

client: src/client.c src/requests.c src/helpers.c src/buffer.c src/parson.c src/utils.c
	@$(CC) -o client src/client.c src/requests.c src/helpers.c src/buffer.c src/parson.c src/utils.c -Wall

run: client
	@echo "Client is running..."
	@./client

clean:
	rm -f *.o client

memory: clean client
	valgrind $(MFLAGS) ./client

beauty: 
	clang-format -i -style=file src/*.c
	clang-format -i -style=file include/*.h
