CC = g++ 
CFLAGS = -g -std=c++17 -Wall -Wextra -Wno-unused-parameter
IP = 127.0.0.1
PORT = 8010
ID = WansE

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
	
run_subscriber: clean subscriber
	@echo "New client connected to the server..."
	@./subscriber ${ID} ${IP} ${PORT}

udp:
	python3 ./checker/udp_client.py --source-port 1234 --input_file ./checker/sample_payloads.json --delay 10 --mode all_once $(IP) $(PORT)

MFLAGS= --leak-check=full --show-leak-kinds=all --track-origins=yes
memory: clean build
	valgrind $(MFLAGS) ./server $(PORT)