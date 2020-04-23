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
    vector<bool> sf;
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
    size_t found;
    if (buffer[0] == 's') {
        found = str.find("subscribe");
        Assert(found == string::npos, "Usage: subscribe topic sf.");
        substr = str.substr(10, str.size());
        substr.erase(substr.end() - 3, substr.end() - 1);
        cout << "subscribed " << substr;
    } else if (buffer[0] == 'u') {
        found = str.find("unsubscribe");
        Assert(found == string::npos, "Usage: subscribe topic sf.");
        substr = str.substr(12, str.size());
        cout << "unsubscribed " << substr;
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

void subscribe(char *bufCpy, vector<clientTcp> &clients, int j) {
    string str(bufCpy);
    clients[j].topics.push_back(str);
    bufCpy = strtok(nullptr, " ");
    if (bufCpy[0] == '0') {
        clients[j].sf.push_back(false);
    }
    if (bufCpy[0] == '1') {
        clients[j].sf.push_back(true);
    }
    for (long unsigned int x = 0; x < clients[j].topics.size(); x++) {
        cout << clients[j].topics[x];
    }
    cout << endl;
}

void unsubscribe(char *bufCpy, vector<clientTcp> &clients, int j) {
    string str(bufCpy);
    str.erase(str.size() - 1);
    int pos = -1;
    for (long unsigned int x = 0; x < clients[j].topics.size(); x++) {
        if (clients[j].topics[x].compare(str) == 0) {
            pos = x;
        }
    }
    if (pos >= 0) {
        clients[j].topics.erase(clients[j].topics.begin() + pos);
        clients[j].sf.erase(clients[j].sf.begin() + pos);
    }
    for (long unsigned int x = 0; x < clients[j].topics.size(); x++) {
        cout << clients[j].topics[x];
    }
    cout << endl;
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
#endif  // SERV_UTILS_H