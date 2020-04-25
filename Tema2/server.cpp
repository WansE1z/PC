#include "include/serv_utils.hpp"

int main(int argc, char *argv[]) {
  int sockTCP, sockUDP, sockTCPnew, portNr, bytesRecv, verify, subCont = 0,
                                                               fdmax, value;
  char buffer[BUFLEN];    // the buffer used for receiveing and sending
  bool exitFlag = false;  // flag used for exitting the server
  socklen_t lenTCP = sizeof(sockaddr), lenUDP = sizeof(sockaddr);
  // the length of addrTCP --- length of addrUDP
  sockaddr_in addrTCP, addrUDP, addrTCPnew;
  vector<subscriber> subscribers(MAX_SUBSCRIBERS);
  /*
      the vector of clients that stores all the information about each client
      For example : topic / status(online/offline)
  */
  fd_set multRead;  // reading lot used for select()
  fd_set multTmp;   // temporary used lot

  Assert(argc != 2, "Usage: ./server <PORT>.\n");
  /*
      checking if the usage is incorrect
      the usage should be always correct due to the fact that i have a command
        in makefile
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
  // populating the tcp address with the values needed for each variable

  addrUDP.sin_family = AF_INET;
  addrUDP.sin_port = htons(portNr);
  addrUDP.sin_addr.s_addr = INADDR_ANY;
  // populating the udp address with the values needed for each variable

  verify = bind(sockTCP, (sockaddr *)&addrTCP, sizeof(sockaddr));
  Assert(verify < 0, "Can't bind TCP socket.\n");
  // checking if the binding for the tcp socket is made in the correct way

  verify = bind(sockUDP, (sockaddr *)&addrUDP, sizeof(sockaddr));
  Assert(verify < 0, "Can't bind UDP socket.\n");
  // checking if the binding for the udp socket is made in the correct way

  verify = listen(sockTCP, MAX_SUBSCRIBERS);
  Assert(verify < 0, "Can't listen the TCP socket.\n");
  // checking if the listening on the socket tcp is made in the correct way

  FD_ZERO(&multRead);
  FD_SET(sockUDP, &multRead);
  FD_SET(sockTCP, &multRead);
  FD_SET(0, &multRead);
  // setting values of the fd's sockets

  fdmax = sockTCP;
  const int options = 1;
  setsockopt(sockTCP, SOL_SOCKET, TCP_NODELAY, &options, sizeof(options) == 0);
  // setting options for the socket reffered to by the file descriptor socket
  // tcp.

  while (1) {
    if (exitFlag) {
      subscribers.clear();
      subscribers.shrink_to_fit();
      closeAllSockets(fdmax);
      break;
    }
    /*
     if the exitFlag is true, then it was received a command to exit the program
     the server will run until this condition is met
    */
    multTmp = multRead;
    // updating the lot

    Assert(select(fdmax + 1, &multTmp, NULL, NULL, NULL) < 0, "Can't select.");
    // if the select is not configured right, then exit the app via the macro

    for (int i = 0; i <= fdmax; i++) {
      // iterating through all file descriptors
      if (FD_ISSET(i, &multTmp)) {
        value = returnSwitch(i, sockTCP, sockUDP);
        // this function returns the values used for the switch
        /*
          1 - TCPCONNECTION
          2 - UDPCONNECTION
          3 - EXIT COMMAND
          4 - SUBSCRIBE AND UNSUBSCRIBE
        */
        switch (value) {
          case TCPCONNECTION:
            sockTCPnew = accept(sockTCP, (sockaddr *)&addrTCPnew, &lenTCP);
            Assert(sockTCPnew < 0, "Can't be accepted.");
            // it was received a new TCP connection

            FD_SET(sockTCPnew, &multRead);
            // setting the new socket

            bzero(&buffer, BUFLEN);
            bytesRecv = recv(sockTCPnew, buffer, sizeof(buffer), 0);
            Assert(bytesRecv < 0, "Id wasn't sent.");

            // the function below updates the client (id,socket)
            updateClient(buffer, sockTCPnew, subCont, subscribers);

            // connecting to the server
            connectServer(addrTCPnew, bytesRecv, subCont, subscribers);

            // as the number of clients has increased, i increment the counter
            subCont++;

            // update as the biggest socket value
            fdmax = updateFD(sockTCPnew, fdmax);
            break;
          case UDPCONNECTION:
            // here i receive the data sent from the generator
            bytesRecv = recvfrom(sockUDP, buffer, BUFLEN, 0,
                                 (sockaddr *)&addrUDP, &lenUDP);
            Assert(bytesRecv < 0,
                   "There was no info received from the UDP socket.\n");
            messageUdp msg;
            memcpy(&msg, buffer, bytesRecv);
            char message[BUFLEN];
            bzero(&message, BUFLEN);
            strcpy(msg.topic, buffer);
            strcpy(msg.ip, inet_ntoa(addrUDP.sin_addr));
            msg.port = ntohs(addrUDP.sin_port);
            msg.data_type = *(uint8_t *)(buffer + 50);
            if (bytesRecv > 0) {
              if (msg.data_type == 0) {
                strcat(message, "INT - ");
                if (*(uint8_t *)(buffer + 51) == 1) {
                  strcat(message, "-");
                }
                strcat(message,
                       to_string(ntohl(*(uint32_t *)(buffer + 52))).c_str());
              }
              if (msg.data_type == 1) {
                strcat(message, "SHORT_REAL - ");
                string helper;
                helper = to_string(ntohs(*(uint16_t *)(buffer + 51)) / 100.0);
                helper.erase(helper.end() - 4, helper.end());
                strcat(message, helper.c_str());
              }
              if (msg.data_type == 2) {
                float decimal = 1;
                strcat(message, "FLOAT - ");
                if (*(int8_t *)(buffer + 51) == 1) {
                  strcat(message, "-");
                }
                for (uint8_t j = 0; j < *(uint8_t *)(buffer + 56); j++) {
                  decimal *= 10.0;
                }
                string helper;
                if (decimal == 1) {
                  helper = to_string(ntohl(*(uint32_t *)(buffer + 52)));
                } else {
                  helper =
                      to_string(ntohl(*(uint32_t *)(buffer + 52)) / decimal);
                  helper.erase(helper.end() - (6 - *(uint8_t *)(buffer + 56)),
                               helper.end());
                }
                strcat(message, helper.c_str());
              }
              if (msg.data_type == 3) {
                strcpy(message, "");
                string str(buffer + 51);
                strcat(message, str.c_str());
              }
            }
            memset(msg.data, 0, sizeof(msg));
            strcat(msg.data, message);
            // sending the information to all of the clients
            for (int x = 0; x < subCont; x++) {
              send(subscribers[x].socket, (char *)&msg, sizeof(messageUdp), 0);
            }
            memset(msg.topic, 0, sizeof(msg));
            memset(msg.data, 0, sizeof(msg));
            break;
          case EXITAPP:
            // in this case i == 0, so there was an exit command received
            exitFlag = exitFunction(buffer);
            if (exitFlag) break;
            break;
          case SUBANDUNSUB:
            bzero(&buffer, BUFLEN);
            bytesRecv = recv(i, buffer, sizeof(buffer), 0);

            /*
             right now in buffer i have 0 bytes, so this means the client
             disconnected or i received a subscribe/unsubscribe command, so the
             server knows
            */

            if (bytesRecv == 0) {
              disconnectServer(subscribers, i, subCont);

              /*
               this line is used in order to not have a problem of a bad file
               descriptor as "Can't select. : Bad file descriptor"
              */
              FD_CLR(i, &multRead);
            } else {
              /*
               the main function of subscribing and unsubscribing
               this function provides two other functions : subscribe and
               unsubscribe more documentation about those functions will be where
               they are defined
              */
              subscribeAndUnsubscribe(subscribers, buffer, subCont, i);
            }
            break;
        }
      }
    }
  }
  return 0;
}