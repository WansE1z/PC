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
#define MSGLEN 1600           // the message's max length
#define MAX_SUBSCRIBERS 1000  // the max number of clients
#define DISCONNECT 0          // bytes received = 0

// the values used below are for the switch
#define TCPCONNECTION 1
#define UDPCONNECTION 2
#define EXITAPP 3
#define SUBANDUNSUB 4

// used for the switch in the udp parsing method
#define INT 0
#define SHORT_REAL 1
#define FLOAT 2
#define STRING 3

/*
  the structure below stores the udp message
  it has topic, that stores the topic the client is subbing to
  data_type, which can be 0/1/2/3
  0 - INT
  1 - SHORT_REAL
  2 - FLOAT
  3 - STRING
  ip, used for the message and port as well for message.
*/

struct messageUdp {
  char topic[50];
  unsigned int data_type;
  char data[1520];
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
  vector<int> last_message;
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
      flag = 1;  // in order to print a %s, it was need to be casted.
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
                   vector<subscriber> &clients,
                   map<string, vector<messageUdp>> subscribersMsg, int sockfd) {
  int counter = 0, flag = 0;
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
      for (int i = 0; i < clientCount; i++) {
        /*
         if the client is already connected, do not connect him again, and set
         his status to online
         i had to cast to char* because the id from clients structure is a
         vector, and in order to use strcmp i had to convert them to char*
        */
        if (strcmp(reinterpret_cast<char *>(clients[i].id.data()),
                   reinterpret_cast<char *>(clients[clientCount].id.data())) ==
            0) {
          /*
            set the status of the client to online
            the counter keeps track of the id of the user
            flag is used in order to print that the client has connected
          */
          clients[i].status = true;
          counter = i;
          flag = 1;
          printf("%s is already connected.\n",
                 reinterpret_cast<char *>(clients[counter].id.data()));
        }
      }
      // needed to cast in order to print %s
      if (flag == 0) {
        printf("New client %s connected from %s:%hu.\n",
               reinterpret_cast<char *>(clients[clientCount].id.data()),
               inet_ntoa(addrTCPnew.sin_addr), ntohs(addrTCPnew.sin_port));

        // update client's status to online
        clients[clientCount].status = true;
      }

      // iterate through the topics, and i send to the clients who are subbed at
      // those topics with the specific sfs
      for (long unsigned int i = 0; i < clients[counter].topics.size(); i++) {
        for (long unsigned int k = clients[counter].last_message[i] + 1;
             k < subscribersMsg[clients[counter].topics[i]].size(); k++) {
          send(sockfd, &subscribersMsg[clients[counter].topics[i]][k],
               sizeof(subscribersMsg[clients[counter].topics[i]][k]), 0);
        }

        // update the clients last message position
        clients[counter].last_message[i] =
            subscribersMsg[clients[counter].topics[i]].size() - 1;
      }
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
    // sf needed, keep the last message
    clients[j].last_message.push_back(0);
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

void parsingUDP(messageUdp &msg, char message[], char buffer[]) {
  uint8_t sign = *(uint8_t *)(buffer + 51);
  switch (msg.data_type) {
    // the message array stores what will be shown in the client terminal
    case INT: {
      strcat(message, "- INT - ");

      // if the sign is 1, which means the bit is set, then add a -
      if (sign == 1) {
        strcat(message, "-");
      }

      /*
        add to the char array the rest of the "data array", which means from
        52-1550 bytes.
      */
      strcat(message, to_string(ntohl(*(uint32_t *)(buffer + 52))).c_str());
      break;
    }
    case SHORT_REAL: {
      strcat(message, "- SHORT_REAL - ");

      // i have created an auxiliary string in order to manage the string
      string helper;
      float res;
      uint16_t value = ntohs(*(uint16_t *)(buffer + 51));

      /*
       value is storing the short int value from buffer
       the number is multiplied by 100 in ordered to be stored as an int
       so we have to divide it to 100.
      */
      res = (float)value;
      res = res / 100.0;
      helper = to_string(res);

      /*
       the division gives us 6 decimals, but we need only 2, so i delete the
       last four ones that are in the string
      */
      helper.erase(helper.end() - 4, helper.end());

      // helper.c_str() converts from string to char[]
      strcat(message, helper.c_str());
      break;
    }
    case FLOAT: {
      string helper;
      float power = 1;
      int flag = 0;
      uint8_t exp = *(uint8_t *)(buffer + 56);
      uint32_t number = *(uint32_t *)(buffer + 52);
      strcat(message, "- FLOAT - ");

      // if the sign is 1, which means the bit is set, then add a -
      if (sign == 1) {
        strcat(message, "-");
      }

      // variable that stores the power of the 10^exp
      power = pow(10, exp);

      // the conditions below return the right string
      if (power != 1) {
        helper = to_string(ntohl(number) / power);
        flag = 1;
      } else {
        helper = to_string(ntohl(number));
      }

      if (flag == 1){
        // delete the last two decimals in order to keep the first 4
        helper.erase(helper.end() - 2, helper.end());
      }

      strcat(message, helper.c_str());
      break;
    }
    case STRING: {
      strcat (message, "- STRING - ");
      string str(buffer + 51);
      // using the string constructor, make a string containing the payload

      // if the size of the string is more than 1500, resize it
      if (str.size() > 1500) {
        str.resize(1500);
      }
      // concat the new string to the char array
      strcat(message, str.c_str());

      break;
    }
  }
}

#endif  // SERV_UTILS_H