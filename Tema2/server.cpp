#include "include/helpers.h"
#include "include/serv_utils.h"
#include <string.h>
#include <vector>
#include <map>
int main(int argc, char *argv[]) {
    int sockTCP, sockUDP, newsockfd, portno;
    char buffer[BUFLEN];
    bool exitFlag = false;
    struct sockaddr_in addrTCP, addrUDP, newAddrTCP;
    int bytesRecv, ret;
    socklen_t lenTCP = sizeof(sockaddr), lenUDP = sizeof(sockaddr);
    int clientCount = 0;
    vector<clientTcp> clients(MAX_CLIENTS);

    fd_set read_fds;  // multimea de citire folosita in select()
    fd_set tmp_fds;   // multime folosita temporar
    int fdmax;        // valoare maxima fd din multimea read_fds

    AssertTrue(argc != 2, "Usage: ./server <PORT>.\n");

    sockTCP = socket(AF_INET, SOCK_STREAM, 0);
    AssertTrue(sockTCP < 0, "The TCP socket can't be created.\n");

    sockUDP = socket(AF_INET, SOCK_DGRAM, 0);
    AssertTrue(sockUDP < 0, "The UDP socket can't be created.\n");

    portno = atoi(argv[1]);
    AssertTrue(portno == 0, "The port number is not correct.\n");

    addrTCP.sin_family = AF_INET;
    addrTCP.sin_port = htons(portno);
    addrTCP.sin_addr.s_addr = INADDR_ANY;

    addrUDP.sin_family = AF_INET;
    addrUDP.sin_port = htons(portno);
    addrUDP.sin_addr.s_addr = INADDR_ANY;

    ret = bind(sockTCP, (sockaddr *)&addrTCP, sizeof(sockaddr));
    AssertTrue(ret < 0, "Can't bind TCP socket.\n");
    ret = bind(sockUDP, (sockaddr *)&addrUDP, sizeof(sockaddr));
    AssertTrue(ret < 0, "Can't bind UDP socket.\n");

    ret = listen(sockTCP, MAX_CLIENTS);
    AssertTrue(ret < 0, "Can't listen the TCP socket.\n");

    FD_ZERO(&read_fds);
    FD_SET(sockUDP, &read_fds);
    FD_SET(sockTCP, &read_fds);
    FD_SET(0, &read_fds);
    fdmax = sockTCP;

    while (1) {
        if (exitFlag) {
            break;
        }
        tmp_fds = read_fds;
        ret = select(fdmax + 1, &tmp_fds, nullptr, nullptr, nullptr);
        AssertTrue(ret < 0, "Can't select.");
        for (int i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &tmp_fds)) {
                if (i == sockTCP) {
                    newsockfd =
                        accept(sockTCP, (sockaddr *)&newAddrTCP, &lenTCP);
                    AssertTrue(newsockfd < 0, "Can't be accepted.");

                    // se adauga noul socket intors de accept() la multimea
                    // descriptorilor de citire
                    FD_SET(newsockfd, &read_fds);
                    if (newsockfd > fdmax) {
                        fdmax = newsockfd;
                    }
                    memset(buffer, 0, BUFLEN);
                    bytesRecv = recv(newsockfd, buffer, sizeof(buffer), 0);
                    AssertTrue (bytesRecv < 0,
                    "There was no information received from TCP socket.");
                    if (bytesRecv > 0){
                        printf("New client %s connected from %s:%hu.\n",
                           buffer,
                           inet_ntoa(newAddrTCP.sin_addr),
                           ntohs(newAddrTCP.sin_port));
                    }
                    strcpy(clients[clientCount].id, buffer);
                    clients[clientCount].status = true;
                    clients[clientCount].socket = newsockfd;
                    clientCount++;
                } else if (i == sockUDP) {
                    ret = recvfrom(sockUDP, buffer, BUFLEN, 0,
                                   (sockaddr *)&addrUDP, &lenUDP);
                    AssertTrue(
                        ret < 0,
                        "There was no info received from the UDP socket.\n");
                } else if (i == 0) {                 
                    fgets(buffer, BUFLEN - 1, stdin);
                    if (strncmp(buffer, "exit", 4) == 0) {
                        exitFlag = true;
                        break;
                    }
                } else {
                    // s-au primit date pe unul din socketii de client,
                    // asa ca serverul trebuie sa le receptioneze
                    memset(buffer, 0, BUFLEN);
                    bytesRecv = recv(i, buffer, sizeof(buffer), 0);
                    AssertTrue(
                        bytesRecv < 0,
                        "There was no information received from TCP socket.");

                    if (bytesRecv == 0) {
                        // conexiunea s-a inchis
                        for (int j = 0; j < clientCount; j++){
                            if (clients[j].socket == i && clients[j].status == true){
                                printf("Client %s disconnected.\n", clients[j].id);
                                clients[j].status = false;
                            }
                        }
                        close(i);
                        // se scoate din multimea de citire socketul inchis
                        FD_CLR(i, &read_fds);
                    } else {
                        string id;
                        char *aux;
                        aux = strtok(buffer, " ");
                        aux = strtok(nullptr, " ");
                        for (int j = 0; j < clientCount; j++){
                            if (clients[j].socket == i && clients[j].status == true){
                                if(buffer[0] == 's') {
                                    clients[j].topics.push_back(aux);
                                    aux = strtok(nullptr, " ");
                                    if(aux[0] == '0') {
                                        clients[j].sf.push_back(false);
                                    }
                                    if(aux[0] == '1') {
                                        clients[j].sf.push_back(true);
                                    }
                                }
                                if(buffer[0] == 'u'){
                                    string str(aux);
                                    str.erase(str.size()-1);
                                    int pos = -1;
                                    for(long unsigned int x = 0; x < clients[j].topics.size(); x++) {
                                        if(clients[j].topics[x].compare(str) == 0) {
                                            pos = j;
                                        }
                                    }
                                    if(pos >= 0) {
                                        clients[j].topics.erase(clients[j].topics.begin() + pos);
                                        clients[j].sf.erase(clients[j].sf.begin() + pos);
                                    }
                                }
                            }	
                        }
                    }
                }
            }
        }
    }
    for (int i = 0; i <= fdmax; i++){
        close(i);
    }
    return 0;
}