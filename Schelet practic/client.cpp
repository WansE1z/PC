#include "include/client_utils.hpp"

int main(int argc, char *argv[]) {
  int sockTCP,sockUDP, bytesRecv, ret;
  sockaddr_in serv_addr;
  sockaddr_in addrUDP;
  socklen_t len = sizeof(addrUDP);
  char buffer[BUFLEN];
  bool exitFlag = false;

  fd_set multRead; // reading lot used for select()
  fd_set multTmp;  // temporary used lot

  FD_ZERO(&multRead);
  FD_ZERO(&multTmp);
  FD_SET(0, &multRead);
  FD_SET(1, &multRead);

  sockTCP = socket(AF_INET, SOCK_STREAM, 0);
  Assert(sockTCP < 0, "Can't open the server socket.\n");

  sockUDP = socket(AF_INET, SOCK_DGRAM, 0);
  Assert(sockUDP < 0, "Can't open the server socket.\n");

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(atoi(argv[2]));
  ret = inet_aton(argv[1], &serv_addr.sin_addr);
  Assert(ret == 0, "The ip that was introduced isn't right.\n");

  ret = connect(sockTCP, (sockaddr *)&serv_addr, sizeof(serv_addr));
  Assert(ret < 0, "Can't connect to the server.\n");

  FD_SET(sockTCP, &multRead);
  FD_SET(sockUDP, &multRead);

  while (1) {
    if (exitFlag) {
      break;
    }
    multTmp = multRead;

    ret = select(sockTCP + 1, &multTmp, NULL, NULL, NULL);
    Assert(ret < 0, "Unable to select.");

    if (FD_ISSET(0, &multTmp)) {
      bzero(&buffer, BUFLEN);
      fgets(buffer, BUFLEN - 1, stdin);

      if (strncmp(buffer, "exit", 4) == 0) {
        exitFlag = true;
      } else {
        exitFlag = false;
      }

      if (exitFlag)
        break;
      send(sockTCP, buffer, BUFLEN, 0);
      Assert(sendto(sockUDP, buffer, BUFLEN, 0, (sockaddr *)&serv_addr, sizeof(serv_addr)) < 0, "Problem.");
    }
    if (FD_ISSET(sockTCP, &multTmp)) {
      bzero(&buffer, BUFLEN);

      bytesRecv = recv(sockTCP, buffer, BUFLEN, 0);

      Assert(bytesRecv < 0, "Number of bytes should be greater than 0.\n");
      if (bytesRecv == 0) {
        close(sockTCP);
        return 0;
      }

      cout << buffer; // informatia de la server e scrisa in client
    }
    if (FD_ISSET(sockUDP, &multTmp)){
      bzero(&buffer, BUFLEN);

      bytesRecv = recvfrom(sockUDP, buffer, BUFLEN, 0, (sockaddr *)&addrUDP,
                    &len);

      Assert(bytesRecv < 0, "Number of bytes should be greater than 0.\n");
      if (bytesRecv == 0) {
        close(sockUDP);
        return 0;
      }

      cout << buffer; // informatia de la server e scrisa in client
    }
  }
  close(sockTCP);
  return 0;
}
