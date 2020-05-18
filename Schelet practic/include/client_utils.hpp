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

#include <map>
#include <vector>

/*
  macro used in order to exit the application in case something
  that is definitely not allowed happens
*/

#define Assert(assertion, call_description)                                    \
  do {                                                                         \
    if (assertion) {                                                           \
      perror(call_description);                                                \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
  } while (0)

/*
  macro used in order to check if an assertion happens
  if it does happen, it prints in the terminal the error in call_description
*/

#define AssertCheck(assertion, call_description)                               \
  do {                                                                         \
    if (assertion) {                                                           \
      perror(call_description);                                                \
    }                                                                          \
  } while (0)

#define BUFLEN 1600      // the message's max length
#define MAX_CLIENTS 1000 // the max number of clients
#define DISCONNECT 0     // bytes received = 0

// the values used below are for the switch
#define TCPCONNECTION 1
#define UDPCONNECTION 2
#define EXITAPP 3
#define OTHERCOMMANDS 4

struct client {
  int socket;
  bool status;
};

// function that closes the socket
void closePort(int socket) {
  shutdown(socket, SHUT_RDWR);
  close(socket);
}

// iterate through all the sockets, and close all of them
void closeAllSockets(int fdmax) {
  for (int i = 0; i <= fdmax; i++) {
    closePort(i);
  }
}

void connectServer(sockaddr_in addrTCPnew, int bytesRecv, int clientCount,
                   vector<client> &clients, int sockfd) {
  /*
   if the number of bytes received is greater than 0, that means that the id
   was sent to the server, so we can post it.
  */
  if (bytesRecv > 0) {
    if (clients[clientCount].status == false) {
      /*
        if the client's status is false, then we connect him and make the status
        true, if not, that means the client is connected, so there is
        no reason to connect him twice.
      */
      // needed to cast in order to print %s
      printf("New client connected from %s:%hu.\n",
             inet_ntoa(addrTCPnew.sin_addr), ntohs(addrTCPnew.sin_port));

      // update client's status to online
      clients[clientCount].status = true;
    }
  }
}

void disconnectServer(vector<client> &clients, int i, int clientCount) {
  for (int j = 0; j < clientCount; j++) {
    /*
     if the socket is good one and the client's status is online
     then i should disconnect him
    */

    if (clients[j].socket == i && clients[j].status == true) {
      printf("Client disconnected.\n");

      clients[j].status = false; // update the client's status to offline
    }
  }
  close(i); // close the socket, as it won't be used anymore
}

bool exitFunction(char buffer[]) {
  fgets(buffer, BUFLEN - 1, stdin);
  // if the command received is to exit, then exit the while.
  if (strncmp(buffer, "exit", 4) == 0) {
    return true;
  }
  return false;
}

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
    value = 4;
  }
  return value;
}

// this function updates the max file descriptor
int updateFD(int sockTCPnew, int fdmax) {
  if (sockTCPnew > fdmax) {
    fdmax = sockTCPnew;
  }
  return fdmax;
}

#endif // SERV_UTILS_H