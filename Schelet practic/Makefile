CC = g++ 
CFLAGS = -g -std=c++17 -Wall -Wextra -Wno-unused-parameter
MFLAGS= --leak-check=full --show-leak-kinds=all --track-origins=yes
IP = 127.0.0.1
PORT = 8000

build: server client

server: server.o
	$(CC) $^ -o $@

client: client.o
	$(CC) $^ -o $@

%.o: %.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	-@rm -f *.o server client

beauty:
	clang-format -i -style=file *.cpp
	clang-format -i -style=file */*.hpp

run_server: clean server
	@echo "Server is running..."
	@./server ${PORT}
	
run_client: clean client
	@echo "New client connected to the server..."
	@./client ${IP} ${PORT}

memory: clean build
	valgrind $(MFLAGS) ./server $(PORT)