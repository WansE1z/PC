#include "include/serv_utils.hpp"

int main(int argc, char *argv[]) {
    int sockTCP, sockUDP, sockTCPnew, portNr, bytesRecv, verify, clientCount = 0, fdmax, value;
    char buffer[BUFLEN];    // the buffer used for receiveing and sending
    bool exitFlag = false;  // flag used for exitting the server
    char id[11];
    socklen_t lenTCP = sizeof(sockaddr), lenUDP = sizeof(sockaddr);
    // the length of addrTCP --- length of addrUDP
    sockaddr_in addrTCP, addrUDP, addrTCPnew;
    vector<clientTcp> clients(MAX_CLIENTS);
    /*
        the vector of clients that stores all the information about each client
        For example : topic / status(online/offline)
    */
    fd_set multRead;  // reading lot used for select()
    fd_set multTmp;   // temporary used lot

    Assert(argc != 2, "Usage: ./server <PORT>.\n");
    /*
        checking if the usage is incorrect
        the usage should be always correct due to the fact that i have a command in makefile
    */
    sockTCP = socket(AF_INET, SOCK_STREAM, 0);
    Assert(sockTCP < 0, "The TCP socket can't be created.\n");
    // creating socket for TCP connection

    sockUDP = socket(AF_INET, SOCK_DGRAM, 0);
    Assert(sockUDP < 0, "The UDP socket can't be created.\n");
    // creating socket for UDP connection

    portNr = atoi(argv[1]);
    Assert(portNr == 0, "The port number is not correct.\n");
    // checking if the port number received is correct

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
                value = returnSwitch(i, sockTCP, sockUDP);
                switch (value) {
                    case 1:
                        sockTCPnew = accept(sockTCP, (sockaddr *)&addrTCPnew, &lenTCP);
                        Assert(sockTCPnew < 0, "Can't be accepted.");
                        FD_SET(sockTCPnew, &multRead);
                        fdmax = updateFD(sockTCPnew, fdmax);
                        memset(buffer, 0, BUFLEN);
                        bytesRecv = recv(sockTCPnew, buffer, sizeof(buffer), 0);
                        Assert(bytesRecv < 0, "Id wasn't sent.");
                        updateClient(id, buffer, sockTCPnew, clientCount, clients);
                        connectServer(addrTCPnew, bytesRecv, id);
                        clientCount++;
                        break;
                    case 2:
                        bytesRecv = recvfrom(sockUDP, buffer, BUFLEN, 0, (sockaddr *)&addrUDP, &lenUDP);
                        Assert(bytesRecv < 0, "There was no info received from the UDP socket.\n");
                        for (int x = 0; x < clientCount; x++) {
                            send(clients[x].socket, buffer, strlen(buffer), 0);
                        }
                        break;
                    case 3:
                        exitFlag = exitFunction(buffer);
                        if (exitFlag) break;
                        break;
                    case 4:
                        memset(buffer, 0, BUFLEN);
                        bytesRecv = recv(i, buffer, sizeof(buffer), 0);
                        if (bytesRecv == 0) {
                            disconnectServer(clients, i, clientCount);
                            FD_CLR(i, &multRead);
                        } else {
                            char *bufCpy;
                            bufCpy = strtok(buffer, " ");
                            bufCpy = strtok(NULL, " ");
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
                        break;
                }
            }
        }
    }
    for (auto i : clients) {
        for (auto j : i.topics) {
            cout << j << " ";
        }
    }
    cout << endl;
    closeAllSockets(fdmax);
    return 0;
}