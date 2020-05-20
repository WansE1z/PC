#ifndef SERV_UTILS_H
#define SERV_UTILS_H

using namespace std;
#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define Assert(assertion, call_description)                                    \
  do {                                                                         \
    if (assertion) {                                                           \
      perror(call_description);                                                \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
  } while (0)

#define AssertCheck(assertion, call_description)                               \
  do {                                                                         \
    if (assertion) {                                                           \
      perror(call_description);                                                \
    }                                                                          \
  } while (0)

#define BUFLEN 1600      // the message's max length
#define MAX_CLIENTS 1000 // the max number of clients

// the values used below are for the switch
#define TCPCONNECTION 1
#define UDPCONNECTION 2
#define EXITAPP 3
#define OTHERCOMMANDS 4

struct client {
  int socket;
  bool voted = false;
};

// function used for the switch
int returnSwitch(int i, int socket1, int socket2) {
  int value;
  if (i == socket1) {
    // socket = TCP
    value = 1;
  } else if (i == socket2) {
    // socket = UDP
    value = 2;
  } else if (i == 0) {
    // exit command
    value = 3;
  } else {
    // other commands
    value = 4;
  }
  return value;
}

#endif // SERV_UTILS_H