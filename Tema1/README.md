# Router

This is a project that represent the first homework for the Communication
 Protocols Course.

# Important note	

On the local checker, i get 95 points, but on vmchecker i couldn't get better
result than 90 points. I don't change anything in the zip, i just resubmit it.
Here is proof of my actual result.
I submitted it last time at 4.04 / 22:00 PM, and I thought the result is the
correct one when i saw 90, but I was too tired and I didn't realise the result
that it should appear would be 95.
So that's why I will resubmit now, and hopefully you understand my problem. 

Here is proof of the result:
![](https://i.imgur.com/EjmX3Gr.png)



## Project structure

``` bash
|-- include
|      |-- arp_parser.h
|      |-- list.h
|      |-- parser.h
|      |-- queue.h
|      |-- skel.h
|-- arp_parser.c
|-- list.c
|-- parser.c
|-- queue.c
|-- router.c
|-- skel.c
```

## Files

- arp_parser.c :
This file contains a count function, that counts the lines of a file,
 and a parser for the arp table, which in my case is static.

- parser.c :
This file contains quicksort function, used to sort the route table in order
 to search binary, the parser for the route table, and also a count function
  with the same functionality as the one in arp_parser.c

- router.c :
This is the main file, containing the main logic of the program. Here i check
 as in the pdf that was given, step by step,the things a router should be able
  to do.

## Router overview

Currently, the router is able to forward packets from every host, but it can't
 forward 10 packets, and I was not able to find out why.
The support for ICMP is implemented, also for IP.

## Installation and Run

In the makefile are multiple rules written, in order to run it faster and better.

make - compiles all the .c files
make distclean - cleans all the .o files
./check.sh - checks the tests

###Other information

It should be opened as a markdown file, in order to get the best out of the file.

 © 2020 Ionita Radu-Alexandru
