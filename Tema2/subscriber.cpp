#include "include/serv_utils.hpp"

int main(int argc, char *argv[]) {
    int sockfd, bytesRecv, ret;
    int idLen = strlen(argv[1]);
    sockaddr_in serv_addr;
    char buffer[BUFLEN];
    bool exitFlag = false;

    Assert(argc != 4, "Usage: ./subscriber <ID> <IP_SERVER> <PORT_SERVER>. \n");
    Assert(idLen > 10, "The ID can't be more than 10 characters.");

    fd_set multRead;  // multimea de citire folosita in select()
    fd_set multTmp;   // multime folosita temporar

    FD_ZERO(&multRead);
    FD_ZERO(&multTmp);

    // Read stdin
    FD_SET(0, &multRead);
    FD_SET(1, &multRead);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    Assert(sockfd < 0, "Can't open the server socket.\n");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[3]));
    ret = inet_aton(argv[2], &serv_addr.sin_addr);
    Assert(ret == 0, "The ip that was introduced isn't right.\n");

    ret = connect(sockfd, (sockaddr *)&serv_addr, sizeof(serv_addr));
    Assert(ret < 0, "Can't connect to the server.\n");
    Assert(send(sockfd, argv[1], idLen + 1, 0) < 0, "Can't send the ID to the server.");

    FD_SET(sockfd, &multRead);
    const int options = 1;
    setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &options, sizeof(options) == 0);
    while (1) {
        if (exitFlag) {
            break;
        }
        multTmp = multRead;
        ret = select(sockfd + 1, &multTmp, NULL, NULL, NULL);
        Assert(ret < 0, "Unable to select.");

        for (int i = 0; i <= sockfd; i += sockfd) {
            if (FD_ISSET(i, &multTmp)) {
                if (i == 0) {
                    memset(buffer, 0, BUFLEN);
                    exitFlag = exitFunction(buffer);
                    if (exitFlag) break;
                    verifySubUnsubCommand(sockfd, buffer);
                } else if (i == sockfd) {
                    memset(buffer, 0, BUFLEN);
                    bytesRecv = recv(sockfd, buffer, BUFLEN, 0);
                    Assert(bytesRecv < 0, "Number of bytes should be greater than 0.\n");
                    if (bytesRecv == 0) {
                        close(sockfd);
                        return 0;
                    }
                    cout << "Received: " << buffer << endl;
                }
            }
        }
    }
    close(sockfd);
    return 0;
}
