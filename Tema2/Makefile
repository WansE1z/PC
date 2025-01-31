CC = g++ 
CFLAGS = -g -std=c++17 -Wall -Wextra -Wno-unused-parameter
MFLAGS= --leak-check=full --show-leak-kinds=all --track-origins=yes
IP = 127.0.0.1
PORT = 8000
ID = WansE
ID1 = Radu

build: server subscriber

server: server.o
	$(CC) $^ -o $@

subscriber: subscriber.o
	$(CC) $^ -o $@

%.o: %.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	-@rm -f *.o server subscriber

beauty:
	clang-format -i -style=file *.cpp
	clang-format -i -style=file */*.hpp

run_server: clean server
	@echo "Server is running..."
	@./server ${PORT}
	
run_subscriber1: clean subscriber
	@echo "New client connected to the server..."
	@./subscriber ${ID} ${IP} ${PORT}

run_subscriber2: clean subscriber
	@echo "New client connected to the server..."
	@./subscriber ${ID1} ${IP} ${PORT}

udp:
	python3 ./checker/udp_client.py --input_file ./checker/sample_payloads.json --delay 10 --mode all_once $(IP) $(PORT)

memory: clean build
	valgrind $(MFLAGS) ./server $(PORT)