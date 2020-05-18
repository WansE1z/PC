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

  // Read stdin
  FD_SET(0, &multRead);
  FD_SET(1, &multRead);

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  Assert(sockfd < 0, "Can't open the server socket.\n");
  // creating socket for the connection

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(atoi(argv[2]));
  ret = inet_aton(argv[1], &serv_addr.sin_addr);
  Assert(ret == 0, "The ip that was introduced isn't right.\n");
  // updating the values for serv_addr

  ret = connect(sockfd, (sockaddr *)&serv_addr, sizeof(serv_addr));
  Assert(ret < 0, "Can't connect to the server.\n");
  // connecting with the server

  FD_SET(sockfd, &multRead);

  while (1) {
    if (exitFlag) {
      break;
    }
    multTmp = multRead;
    // updating the lot

    ret = select(sockfd + 1, &multTmp, NULL, NULL, NULL);
    Assert(ret < 0, "Unable to select.");
    // checking if the select was made in the proper way

    if (FD_ISSET(0, &multTmp)) {
      bzero(&buffer, BUFLEN);
      // declaring the buffer
      exitFlag = exitFunction(buffer);
      /*
        a bool that its purpose is when it is true, to exit the app
        if it receives the exit command, it closes the terminal
      */
      if (exitFlag)
        break;
      send(sockfd, buffer, BUFLEN, 0); // send data to
    }
    if (FD_ISSET(sockfd, &multTmp)) {
      bzero(&buffer, BUFLEN);
      // declaring the buffer
      bytesRecv = recv(sockfd, buffer, BUFLEN, 0);
      // receiving the messages sent from the server
      Assert(bytesRecv < 0, "Number of bytes should be greater than 0.\n");
      if (bytesRecv == 0) {
        // if the number of bytes received is equal to 0, than close the socket
        close(sockfd);
        return 0;
      }
      // print what you receive
      cout << buffer;
    }
  }
  close(sockfd);
  return 0;
}
