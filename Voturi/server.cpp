#include "include/client_utils.hpp"

int main(int argc, char *argv[]) {
  int sockTCP, sockUDP, sockTCPnew, portNr, bytesRecv, verify, clientCount = 0,
                                                               fdmax, value;
  char buffer[BUFLEN];
  bool exitFlag = false;
  socklen_t lenTCP = sizeof(sockaddr), lenUDP = sizeof(sockaddr);
  sockaddr_in addrTCP, addrTCPnew, addrUDP;
  vector<client> clients(MAX_CLIENTS);
  int vote1 = 0, vote0 = 0;
  int flag = 0;
  bool allVoted = false;

  fd_set multRead; // reading lot used for select()
  fd_set multTmp;  // temporary used lot

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
  verify = listen(sockTCP, MAX_CLIENTS);
  Assert(verify < 0, "Can't listen the TCP socket.\n");

  FD_ZERO(&multRead);
  FD_SET(sockTCP, &multRead);
  FD_SET(0, &multRead);
  fdmax = sockTCP;

  while (1) {
    if (exitFlag) {
      for (int i = 0; i <= fdmax; i++) {
        shutdown(i, SHUT_RDWR);
        close(i);
      }
      break;
    }
    multTmp = multRead;

    Assert(select(fdmax + 1, &multTmp, NULL, NULL, NULL) < 0, "Can't select.");

    for (int i = 0; i <= fdmax; i++) {
      if (FD_ISSET(i, &multTmp)) {
        value = returnSwitch(i, sockTCP, sockUDP);
        switch (value) {
        case TCPCONNECTION: {
          sockTCPnew = accept(sockTCP, (sockaddr *)&addrTCPnew, &lenTCP);
          Assert(sockTCPnew < 0, "Can't be accepted.");
          FD_SET(sockTCPnew, &multRead);

          printf("New client connected from %s:%hu.\n",
                 inet_ntoa(addrTCPnew.sin_addr), ntohs(addrTCPnew.sin_port));
          clients[clientCount].socket = sockTCPnew;
          clientCount++;

          if (sockTCPnew > fdmax) {
            fdmax = sockTCPnew;
          }
          break;
        }
        case UDPCONNECTION: {
          bytesRecv = recvfrom(sockUDP, buffer, BUFLEN, 0, (sockaddr *)&addrUDP,
                               &lenUDP);
          Assert(bytesRecv < 0,
                 "There was no info received from the UDP socket.\n");
          break;
        }
        case EXITAPP: {
          fgets(buffer, BUFLEN - 1, stdin);

          if (strncmp(buffer, "exit", 4) == 0) {
            exitFlag = true;
          } else {
            exitFlag = false;
            /*
              trimit la clienti detalii din server
            */
            for (int j = 0; j < clientCount; j++) {
              send(clients[j].socket, buffer, BUFLEN, 0);
            }
          }

          if (exitFlag)
            break;

          break;
        }

        case OTHERCOMMANDS: {
          bzero(&buffer, BUFLEN);
          bytesRecv = recv(i, buffer, sizeof(buffer), 0);

          if (bytesRecv == 0) {
            printf("Client disconnected.\n");
            close(i);
            FD_CLR(i, &multRead);
          } else {
            if (buffer[0] == 'v') {
              flag = 0;
              for (int j = 0; j < clientCount; j++) {
                if (i == clients[j].socket && !clients[j].voted) {
                  flag = 1;
                  clients[j].voted = true;
                }
              }
              if (flag == 1) {
                if (buffer[5] == '1') {
                  vote1++;
                } else if (buffer[5] == '0') {
                  vote0++;
                }
              }
              if (flag == 0) {
                memset(buffer, 0, BUFLEN);
                strcat(buffer, "Your vote has already been registered!\n");
                for (int j = 0; j < clientCount; j++) {
                  if (i == clients[j].socket) {
                    send(clients[j].socket, buffer, BUFLEN, 0);
                  }
                }
              }
            }
            if (buffer[0] == 's') {
              memset(buffer, 0, BUFLEN);
              strcat(buffer, "Candidatul 1 are : ");
              strcat(buffer, to_string(vote1).c_str());
              strcat(buffer, " voturi\n");
              strcat(buffer, "Candidatul 0 are : ");
              strcat(buffer, to_string(vote0).c_str());
              strcat(buffer, " voturi\n");

              for (int j = 0; j < clientCount; j++) {
                if (i == clients[j].socket) {
                  send(clients[j].socket, buffer, BUFLEN, 0);
                }
              }
            }
            allVoted = true;
            for (int j = 0; j < clientCount; j++) {
              if (!clients[j].voted) {
                allVoted = false;
              }
            }
            if (allVoted) {
              memset(buffer, 0, BUFLEN);
              strcat(buffer, "Candidat 0: ");
              strcat(buffer, to_string(vote0).c_str());
              strcat(buffer, "\n");
              strcat(buffer, "Candidat 1: ");
              strcat(buffer, to_string(vote1).c_str());
              strcat(buffer, "\n");
              if (vote1 > vote0) {
                strcat(buffer, "Winner is Candidat 1");
              } else if (vote1 < vote0){
                strcat(buffer, "Winner is Candidat 0");
              } else {
                strcat(buffer, "Tie");
              }
              for (int j = 0; j < clientCount; j++) {
                send(clients[j].socket, buffer, BUFLEN, 0);
              }
            }
          }
          break;
        }
        }
      }
    }
  }
  return 0;
}