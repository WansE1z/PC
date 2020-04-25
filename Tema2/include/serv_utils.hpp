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

#define Assert(assertion, call_description) \
  do {                                      \
    if (assertion) {                        \
      perror(call_description);             \
      exit(EXIT_FAILURE);                   \
    }                                       \
  } while (0)

/*
  macro used in order to check if an assertion happens
  if it does happen, it prints in the terminal the error in call_description
*/

#define AssertCheck(assertion, call_description) \
  do {                                           \
    if (assertion) {                             \
      perror(call_description);                  \
    }                                            \
  } while (0)

#define BUFLEN 1600           // the buffer's maximum length
#define MAX_SUBSCRIBERS 1000  // the max number of clients
#define DISCONNECT 0          // bytes received = 0

// the values used below are for the switch
#define TCPCONNECTION 1
#define UDPCONNECTION 2
#define EXITAPP 3
#define SUBANDUNSUB 4

struct messageUdp {
  char topic[50];
  unsigned int data_type;
  char data[1500];
  char ip[16];
  uint16_t port;
};

/*
 The structure below is used in order to keep track of each subscriber.

 The socket stores the socket that the subscriber has.

 Id is a char array that stores his id.

 Topics is a string array that stores all the topics the client is subbed to.

 Sf is an int array storing the sf received from the subscribe command in order
 to know if we need to show the message after his reconnection or not.

 Status is a boolean variable :
 {
   If the value is true => subscriber is online
   If the value is false => subscribe is offline
 }

*/
struct subscriber {
  int socket;
  vector<char> id;
  vector<string> topics;
  vector<int> sf;
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

// this function prints the feedback from the subscriber
void printFeedback(char buffer[]) {
  string str(buffer);  // using the constructor so i have the buffer in a string
  string substr;
  if (buffer[0] == 's') {
    /*
      For example, i have "subscribe topic 1"
      I extract the first ten characters, so it will be "topic 1"
      Then i remove those 2 characters, in order to remain "topic"
      After that i print it as feedback
    */
    substr = str.substr(10, str.size());
    substr.erase(substr.end() - 3, substr.end() - 1);
    cout << "subscribed " << substr;
  } else if (buffer[0] == 'u') {
    /*
      For example, i have "unsubscribe topic"
      I extract the first twelve characters, so it will be "topic"
      After that i print it as feedback
    */
    substr = str.substr(12, str.size());
    cout << "unsubscribed " << substr;
  }
}

void verifySubUnsubCommand(int sockfd, char buffer[]) {
  size_t found;
  string str(buffer);
  // if flag equals to 1, then it's alright to send the message, else it isn't
  int flag = 0;

  /*
   this is a value i struggled to find, because when i enter the command
   from the terminal, i also include a " ", so i have to remove two characters
   not only one, as I was expecting
  */
  int len = strlen(buffer) - 2;

  if (buffer[0] == 's') {
    // if the first character is 's', it should be the subscribe command

    // return the position where it founds "subscribe"
    found = str.find("subscribe");

    /*
     if the find function returns a number different than string::npos
     it means that the searched string was found, so it exists
    */
    if (found != string::npos) {
      flag = 1;
    }
    if (buffer[len] == '0' || buffer[len] == '1') {
      // the sf must be 0/1, in case it isn't, show error
      flag = 1;
    } else {
      flag = 0;
    }
  }
  if (buffer[0] == 'u') {
    // if the first character is 'u', it should be the unsubscribe command

    // return the position where it founds "unsubscribe"
    found = str.find("unsubscribe");
    if (found != string::npos) {
      flag = 1;
    }
  }
  // if the flag is one, then send the buffer and print feedback
  if (flag == 1) {
    send(sockfd, buffer, strlen(buffer), 0);
    printFeedback(buffer);
  } else {
    /*
     in case the flag is different than one, in our case always zero
     print a message to the client that shows him the correct way to use
     the subscribe or unsubscribe command
    */
    if (buffer[0] == 's') {
      printf(
          "Usage must be: subscribe topic sf (where sf must be 0/1), not "
          "%s",
          buffer);
    } else if (buffer[0] == 'u') {
      printf("Usage must be: unsubscribe topic, not %s", buffer);
    }
  }
}

void connectServer(sockaddr_in addrTCPnew, int bytesRecv, int clientCount,
                   vector<subscriber> &clients) {
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

      printf("New client %s connected from %s:%hu.\n",
             reinterpret_cast<char *>(clients[clientCount].id.data()),
             inet_ntoa(addrTCPnew.sin_addr), ntohs(addrTCPnew.sin_port));
      clients[clientCount].status = true;
    } else {
      printf("The client %s is already connected.",
             reinterpret_cast<char *>(clients[clientCount].id.data()));
      // in order to print a %s, it was need to be casted.
    }
  }
}

void disconnectServer(vector<subscriber> &clients, int i, int clientCount) {
  for (int j = 0; j < clientCount; j++) {
    /*
     if the socket is good one and the client's status is online
     then i should disconnect him
    */

    if (clients[j].socket == i && clients[j].status == true) {
      printf("Client %s disconnected.\n",
             reinterpret_cast<char *>(clients[j].id.data()));
      // in order to print a %s, it was need to be casted.

      clients[j].status = false;  // update the client's status to offline
    }
  }
  close(i);  // close the socket, as it won't be used anymore
}

bool exitFunction(char buffer[]) {
  fgets(buffer, BUFLEN - 1, stdin);
  // if the command received is to exit, then exit the while.
  if (strncmp(buffer, "exit", 4) == 0) {
    return true;
  }
  return false;
}

void printTopics(vector<subscriber> &clients, int j) {
  for (long unsigned int x = 0; x < clients[j].topics.size(); x++) {
    cout << clients[j].topics[x];
  }
  cout << endl;
}

void printSf(vector<subscriber> &clients, int j) {
  for (long unsigned int x = 0; x < clients[j].sf.size(); x++) {
    cout << clients[j].sf[x];
  }
  cout << endl;
}

void subscribe(char *bufCpy, vector<subscriber> &clients, int j) {
  string str(bufCpy);
  // using constructor to make the array of chars a string
  vector<string>::iterator itTopic;
  itTopic = find(clients[j].topics.begin(), clients[j].topics.end(), str);
  if (itTopic == clients[j].topics.end()) {
    clients[j].topics.push_back(str);
    // adding the topic to the array of topics
    bufCpy = strtok(nullptr, " ");
    // now i go over the " " and add the SF's to the vector
    if (bufCpy[0] == '0') {
      clients[j].sf.push_back(0);
    }
    // adds the sf to the client
    if (bufCpy[0] == '1') {
      clients[j].sf.push_back(1);
    }
  } else {
    // in case the topic is found again, it must not be added twice
    AssertCheck(itTopic != clients[j].topics.end(),
                "You are not allowed to subscribe to the same topic twice.");
  }
  // debugging purposes
  printTopics(clients, j);
  printSf(clients, j);
}

void unsubscribe(char *bufCpy, vector<subscriber> &clients, int j) {
  int counter = 0;  // this counter is used in order to keep track of the
                    // position to delete from the vector
  string str(
      bufCpy);  // using the string constructor to have the buffer as string
  str.erase(str.size() - 1);

  // the iterator is used in order to go through all topics
  vector<string>::iterator itTopic;

  itTopic = find(clients[j].topics.begin(), clients[j].topics.end(), str);
  for (auto i = clients[j].topics.begin(); i <= itTopic; i++) {
    counter++;  // update the position
  }

  /*
    if the iterator is not at the end, this means that the topic was found
    itTopic stores the string found, so now we can erase it
    in order to delete also the sf, i have stored a counter above
    that tracks the position, i found the position and delete also the sf
  */

  if (itTopic != clients[j].topics.end()) {
    clients[j].topics.erase(itTopic);
    clients[j].sf.erase(clients[j].sf.begin() + counter - 1);
  } else {
    // if the iterator is at its end, this means the topic was not found, so
    // print error
    AssertCheck(itTopic == clients[j].topics.end(), "The topic doesn't exist.");
  }
  printTopics(clients, j);
  printSf(clients, j);
}

/*
  This function has more detailed explcation and documentation in
  the functions used: subscribe and unsubscribe
*/
void subscribeAndUnsubscribe(vector<subscriber> &clients, char buffer[],
                             int clientCount, int i) {
  // bufCpy is a pointer that stores parts of buffer
  char *bufCpy;
  bufCpy = strtok(buffer, " ");
  bufCpy = strtok(NULL, " ");

  for (int j = 0; j < clientCount; j++) {
    // if the status of the client is true and the socket is the one good
    if (clients[j].socket == i && clients[j].status == true) {
      // if the first char is 's', then it is a subscribe command
      if (buffer[0] == 's') {
        subscribe(bufCpy, clients, j);
      }
      // if the first is 'u', it is a unsubscribe command
      if (buffer[0] == 'u') {
        unsubscribe(bufCpy, clients, j);
      }
    }
  }
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
    // subscribe and unsubscribe commands
    value = 4;
  }
  return value;
}

// this function updates the subscriber's id and socket
void updateClient(char buffer[], int sockTCPnew, int clientCount,
                  vector<subscriber> &clients) {
  vector<char> data(buffer, buffer + (BUFLEN - 1));
  clients[clientCount].id = data;
  clients[clientCount].socket = sockTCPnew;
}

// this function updates the max file descriptor
int updateFD(int sockTCPnew, int fdmax) {
  if (sockTCPnew > fdmax) {
    fdmax = sockTCPnew;
  }
  return fdmax;
}

#endif  // SERV_UTILS_H