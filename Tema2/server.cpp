#include "include/serv_utils.hpp"

int main(int argc, char *argv[]) {
    int sockTCP, sockUDP, sockTCPnew, portNr, bytesRecv, verify, clientCount = 0, fdmax;
    char buffer[BUFLEN];
    bool exitFlag = false;
    char id[11];
    socklen_t lenTCP = sizeof(sockaddr), lenUDP = sizeof(sockaddr);
    sockaddr_in addrTCP, addrUDP, addrTCPnew;
    vector<clientTcp> clients(MAX_CLIENTS);
    fd_set multRead;  // multimea de citire folosita in select()
    fd_set multTmp;   // multime folosita temporar

    Assert(argc != 2, "Usage: ./server <PORT>.\n");
    sockTCP = socket(AF_INET, SOCK_STREAM, 0);
    Assert(sockTCP < 0, "The TCP socket can't be created.\n");
    sockUDP = socket(AF_INET, SOCK_DGRAM, 0);
    Assert(sockUDP < 0, "The UDP socket can't be created.\n");
    portNr = atoi(argv[1]);
    Assert(portNr == 0, "The port number is not correct.\n");

    addrTCP.sin_family = AF_INET;
    addrTCP.sin_port = htons(portNr);
    addrTCP.sin_addr.s_addr = INADDR_ANY;

    addrUDP.sin_family = AF_INET;
    addrUDP.sin_port = htons(portNr);
    addrUDP.sin_addr.s_addr = INADDR_ANY;

    verify = bind(sockTCP, (sockaddr *)&addrTCP, sizeof(sockaddr));
    Assert(verify < 0, "Can't bind TCP socket.\n");

    verify = bind(sockUDP, (sockaddr *)&addrUDP, sizeof(sockaddr));
    Assert(verify < 0, "Can't bind UDP socket.\n");

    verify = listen(sockTCP, MAX_CLIENTS);
    Assert(verify < 0, "Can't listen the TCP socket.\n");

    FD_ZERO(&multRead);
    FD_SET(sockUDP, &multRead);
    FD_SET(sockTCP, &multRead);
    FD_SET(0, &multRead);

    fdmax = sockTCP;
    const int options = 1;
    setsockopt(sockTCP, SOL_SOCKET, TCP_NODELAY, &options, sizeof(options) == 0);

    while (1) {
        if (exitFlag) {
            break;
        }
        multTmp = multRead;
        Assert(select(fdmax + 1, &multTmp, NULL, NULL, NULL) < 0, "Can't select.");
        for (int i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &multTmp)) {
                if (i == sockTCP) {
                    sockTCPnew = accept(sockTCP, (sockaddr *)&addrTCPnew, &lenTCP);
                    Assert(sockTCPnew < 0, "Can't be accepted.");
                    FD_SET(sockTCPnew, &multRead);
                    if (sockTCPnew > fdmax) {
                        fdmax = sockTCPnew;
                    }
                    memset(buffer, 0, BUFLEN);
                    bytesRecv = recv(sockTCPnew, buffer, sizeof(buffer), 0);
                    strcpy(id, buffer);
                    strcpy(clients[clientCount].id, id);
                    clients[clientCount].status = true;
                    clients[clientCount].socket = sockTCPnew;
                    clientCount++;
                    connectServer(addrTCPnew, bytesRecv, id);
                } else if (i == sockUDP) {
                    bytesRecv = recvfrom(sockUDP, buffer, BUFLEN, 0, (sockaddr *)&addrUDP, &lenUDP);
                    Assert(bytesRecv < 0, "There was no info received from the UDP socket.\n");
                } else if (i == 0) {
                    exitFlag = exitFunction(buffer);
                    if (exitFlag) break;
                } else {
                    memset(buffer, 0, BUFLEN);
                    bytesRecv = recv(i, buffer, sizeof(buffer), 0);
                    if (bytesRecv == 0) {
                        disconnectServer(clients, i, clientCount);
                        FD_CLR(i, &multRead);
                    } else {
                        char *bufCpy;
                        bufCpy = strtok(buffer, " ");
                        bufCpy = strtok(nullptr, " ");
                        for (int j = 0; j < clientCount; j++) {
                            if (clients[j].socket == i && clients[j].status == true) {
                                if (buffer[0] == 's') {
                                    subscribe(bufCpy, clients, j);
                                }
                                if (buffer[0] == 'u') {
                                    unsubscribe(bufCpy, clients, j);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    for (auto i : clients){
        for (auto j : i.topics){
            cout << j << " ";
        }
    }
    cout << endl;
    closeAllSockets(fdmax);
    return 0;
}