#include "include/client_utils.hpp"

int main(int argc, char *argv[]) {
  int sockfd, bytesRecv, ret;
  sockaddr_in serv_addr;
  char buffer[BUFLEN];
  bool exitFlag = false;

  fd_set multRead; // reading lot used for select()
  fd_set multTmp;  // temporary used lot

  FD_ZERO(&multRead);
  FD_ZERO(&multTmp);
  FD_SET(0, &multRead);
  FD_SET(1, &multRead);

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  Assert(sockfd < 0, "Can't open the server socket.\n");

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(atoi(argv[2]));
  ret = inet_aton(argv[1], &serv_addr.sin_addr);
  Assert(ret == 0, "The ip that was introduced isn't right.\n");

  ret = connect(sockfd, (sockaddr *)&serv_addr, sizeof(serv_addr));
  Assert(ret < 0, "Can't connect to the server.\n");

  FD_SET(sockfd, &multRead);

  while (1) {
    if (exitFlag) {
      break;
    }
    multTmp = multRead;

    ret = select(sockfd + 1, &multTmp, NULL, NULL, NULL);
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
      send(sockfd, buffer, BUFLEN, 0);
    }
    if (FD_ISSET(sockfd, &multTmp)) {
      bzero(&buffer, BUFLEN);

      bytesRecv = recv(sockfd, buffer, BUFLEN, 0);

      Assert(bytesRecv < 0, "Number of bytes should be greater than 0.\n");
      if (bytesRecv == 0) {
        close(sockfd);
        return 0;
      }

      cout << buffer << endl; // informatia de la server e scrisa in client
    }
  }
  close(sockfd);
  return 0;
}
