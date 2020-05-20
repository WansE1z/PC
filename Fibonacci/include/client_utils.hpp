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
#include  <random>
#include  <iterator>

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

vector<int> fibonacci (int minLimit, int maxLimit){
  vector<int> v;
  int t1 = 0;
  int t2 = 1;
  int nextTerm = 0;
  for (int i = 1;i <= 9999; i++){
        // Prints the first two terms.
        if(i == 1)
        {
            if (minLimit == 0) {
                v.push_back(0);
            }
            continue;
        }
        if(i == 2)
        {
            if (minLimit == 0 || minLimit == 1){
                v.push_back(1);
            }
            continue;
        }
        if (maxLimit <= nextTerm){
            break;
        }
        nextTerm = t1 + t2;
        t1 = t2;
        t2 = nextTerm;
        if (maxLimit <= nextTerm){
            break;
        }
        if (i != 1 || i != 2){
            if (nextTerm >= minLimit){
                v.push_back(nextTerm);
            }
        }
    }
  return v;
}

template<typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(g));
    return start;
}

template<typename Iter>
Iter select_randomly(Iter start, Iter end) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return select_randomly(start, end, gen);
}

vector<int> extractIntegerWords(string str) 
{ 
    stringstream ss;     
    vector<int> v;
    /* Storing the whole string into string stream */
    ss << str; 
  
    /* Running loop till the end of the stream */
    string temp; 
    int found;
    while (!ss.eof()) { 
  
        /* extracting word by word from stream */
        ss >> temp; 
  
        /* Checking the given word is integer or not */
        if (stringstream(temp) >> found) 
             v.push_back(found);
  
        /* To save from space at the end of string */
        temp = ""; 
    } 
  return v;
} 

#endif // SERV_UTILS_H