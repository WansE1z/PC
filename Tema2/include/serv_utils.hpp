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

#define Assert(assertion, call_description)                    \
    do {                                                       \
        if (assertion) {                                       \
            fprintf(stderr, "(%s, %d): ", __FILE__, __LINE__); \
            perror(call_description);                          \
            exit(EXIT_FAILURE);                                \
        }                                                      \
    } while (0)

#define AssertCheck(assertion, call_description)               \
    do {                                                       \
        if (assertion) {                                       \
            fprintf(stderr, "(%s, %d): ", __FILE__, __LINE__); \
            perror(call_description);                          \
        }                                                      \
    } while (0)

#define BUFLEN 1560       // dimensiunea maxima a calupului de date
#define MAX_CLIENTS 1000  // numarul maxim de clienti in asteptare

struct messageUdp {
    char topic[51];
    unsigned int data_type;
    char data[1501];
};

struct clientTcp {
    int socket;
    char id[10];
    vector<string> topics;
    vector<int> sf;
    bool status;  // online/offline
};

void closePort(int socket) {
    shutdown(socket, SHUT_RDWR);
    close(socket);
}

void closeAllSockets(int fdmax) {
    for (int i = 0; i <= fdmax; i++) {
        closePort(i);
    }
}

void printMessage(char buffer[]) {
    string str(buffer);
    string substr;
    if (buffer[0] == 's') {
        substr = str.substr(10, str.size());
        substr.erase(substr.end() - 3, substr.end() - 1);
        cout << "subscribed " << substr;
    } else if (buffer[0] == 'u') {
        substr = str.substr(12, str.size());
        cout << "unsubscribed " << substr;
    }
}

void verifySubUnsubCommand(int sockfd, char buffer[]) {
    size_t found;
    string str(buffer);
    int flag = 0;
    if (buffer[0] == 's') {
        found = str.find("subscribe");
        if (found != string::npos) {
            flag = 1;
        }
    }
    if (buffer[0] == 'u') {
        found = str.find("unsubscribe");
        if (found != string::npos) {
            flag = 1;
        }
    }
    if (flag == 1) {
        send(sockfd, buffer, strlen(buffer), 0);
        printMessage(buffer);
    } else {
        if (buffer[0] == 's') {
            printf("Usage must be: subscribe topic sf, not %s", buffer);
        } else if (buffer[0] == 'u') {
            printf("Usage must be: unsubscribe topic, not %s", buffer);
        }
    }
}

void connectServer(sockaddr_in addrTCPnew, int bytesRecv, char id[]) {
    if (bytesRecv > 0) {
        printf("New client %s connected from %s:%hu.\n", id, inet_ntoa(addrTCPnew.sin_addr),
               ntohs(addrTCPnew.sin_port));
    }
}

void disconnectServer(vector<clientTcp> &clients, int i, int clientCount) {
    for (int j = 0; j < clientCount; j++) {
        if (clients[j].socket == i && clients[j].status == true) {
            printf("Client %s disconnected.\n", clients[j].id);
            clients[j].status = false;
        }
    }
    close(i);
}

bool exitFunction(char buffer[]) {
    fgets(buffer, BUFLEN - 1, stdin);
    if (strncmp(buffer, "exit", 4) == 0) {
        return true;
    }
    return false;
}

void printTopics(vector<clientTcp> &clients, int j) {
    for (long unsigned int x = 0; x < clients[j].topics.size(); x++) {
        cout << clients[j].topics[x];
    }
    cout << endl;
}

void printSf(vector<clientTcp> &clients, int j) {
    for (long unsigned int x = 0; x < clients[j].sf.size(); x++) {
        cout << clients[j].sf[x];
    }
    cout << endl;
}

void subscribe(char *bufCpy, vector<clientTcp> &clients, int j) {
    string str(bufCpy);
    // using constructor to make the array of chars a string
    clients[j].topics.push_back(str);
    // adding the topic to the array of topics
    bufCpy = strtok(nullptr, " ");
    // now i go over the " " and add the SF's to the vector
    if (bufCpy[0] == '0') {
        clients[j].sf.push_back(0);
    }
    if (bufCpy[0] == '1') {
        clients[j].sf.push_back(1);
    }
    printTopics(clients, j);
    printSf(clients, j);
}

void unsubscribe(char *bufCpy, vector<clientTcp> &clients, int j) {
    int x = 0;
    string str(bufCpy);
    str.erase(str.size() - 1);
    vector<string>::iterator itTopic;
    itTopic = find(clients[j].topics.begin(), clients[j].topics.end(), str);
    for (auto i = clients[j].topics.begin(); i <= itTopic; i++) {
        x++;
    }
    if (itTopic != clients[j].topics.end()) {
        clients[j].topics.erase(itTopic);
        clients[j].sf.erase(clients[j].sf.begin() + x - 1);
    }
    printTopics(clients, j);
    printSf(clients, j);
}

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
void updateClient(char id[], char buffer[], int sockTCPnew, int clientCount, vector<clientTcp> &clients) {
    strcpy(id, buffer);
    strcpy(clients[clientCount].id, id);
    clients[clientCount].status = true;
    clients[clientCount].socket = sockTCPnew;
}

int updateFD(int sockTCPnew, int fdmax) {
    if (sockTCPnew > fdmax) {
        fdmax = sockTCPnew;
    }
    return fdmax;
}

#endif  // SERV_UTILS_H