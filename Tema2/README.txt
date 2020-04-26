#Server - Client Homework Nr2 PC

This is a project that represents the second homework for the Communication 
Protocols Course.

## Project structure

``` bash
|-- include
|    |--serv_utils.hpp
|-- checker
|    |--sample_payloads.json
|    |--three_topics_payloads.json
|    |--udp_client.py
|    |--utils
|    |    |--__init__.py
|    |    |--__pycache__
|    |    |    |--__init__.cpython-37.pyc
|    |    |    |--unpriv_port.cpython-37.pyc
|    |    |--unpriv_port.py
|    |--Makefile
|    |--server.cpp
|    |--subscriber.cpp
```

## Files

- server.cpp:
This file contains the main logic behind the subscribe/unsubscribe commands, 
the exit command that closes the server, and checking that the connection, 
select, etc. are made in the correct way.

-subscriber.cpp:
This file contains the logic behind the client's role, which is to subscribe
and unsubscribe to a topic. In case it receives the exit command, it closes
the terminal. In this file i print the subscribe/unsubscribe feedback and 
also the message needed when there is an udp message sent, so the udp message
feedback.

-serv_utils.hpp:
The intention of this file is to be a header, but mainly it stores the methods
 used by the server and also by the subscriber, in order to be more organized 
and the code to be more legible.

## Details about representation of the messages

In order to be efficient, i have made a structure that takes care of this 
problem.The structure clients has a variable called topic, in which i store
 the TCP topics received.

## Server-Client overview

Currently, the server-client is working properlly. By the tests I have made,
 including many cases, there are no major problems. The only problems I am 
concerned of are the fact that if you subscribe to the same topic, the feedback
 will be that you subscribed to the topic, but in the server it appear that you
 didn't, so it is just a mistake of printing, and also if you connect on the 
same id in the same time, will appear that you connected but in fact in the 
server it appears that the id is taken.

## Running the server-client 

The makefile contains several builds including:

make -> it compiles server.cpp and subscriber.cpp

make clean -> cleans server.o and subscriber.o

make beauty -> formats the files in my personal style, but most important
 it keeps the 80 character on a line.

make run_server -> runs the server (./server 8000)

make run_subscriber1 -> runs the client (./subscriber WansE 127.0.0.1 8000)

make run_subscriber2 -> runs another client for debugging and testing purposes
 (./subscriber Radu 127.0.0.1 8000)

make udp -> runs the udp generator, that sends the messages to all the clients
 subbed to a topic

make memory -> runs the server with valgrind on , in order to spot any leaks.

If you want to change the ip, port, or id, there are above some variables 
(IP,PORT,ID), that you can change. In order to run the app without these 
already made commands, you can use :
./server <PORT>
./subscriber <ID> <IP> <PORT>

### Other information

It should be opened as a markdown file, in order to get the best out of 
this file. More details about the implementation are in the comments in 
the files (server.cpp, subscriber.cpp, serv_utils.hpp).

 © 2020 Ionita Radu-Alexandru
