#include "include/helpers.h"

int main(int argc, char *argv[]) {
    int sockfd, bytesRecv, ret;
    int idLen = strlen(argv[1]);
    sockaddr_in serv_addr;
    char buffer[BUFLEN];
    bool exitFlag = false;

    AssertTrue(argc != 4,
               "Usage: ./subscriber <ID> <IP_SERVER> <PORT_SERVER>. \n");
    AssertTrue(idLen > 10, "The ID can't be more than 10 characters.");

    fd_set read_fds;  // multimea de citire folosita in select()
    fd_set tmp_fds;   // multime folosita temporar
    int fdmax;        // valoare maxima fd din multimea read_fds

    FD_ZERO(&read_fds);
    FD_ZERO(&tmp_fds);

    // Read stdin
    FD_SET(0, &read_fds);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    AssertTrue(sockfd < 0, "Can't open the server socket.\n");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[3]));
    ret = inet_aton(argv[2], &serv_addr.sin_addr);
    AssertTrue(ret == 0, "The ip that was introduced isn't right.\n");

    ret = connect(sockfd, (sockaddr *)&serv_addr, sizeof(serv_addr));
    AssertTrue(ret < 0, "Can't connect to the server.\n");
    AssertTrue(send(sockfd, argv[1], idLen + 1, 0) < 0,
               "Can't send the ID to the server.");

    FD_SET(sockfd, &read_fds);
    fdmax = sockfd;

    while (1) {
        if (exitFlag) {
            break;
        }
        tmp_fds = read_fds;

        ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
        AssertTrue(ret < 0, "Unable to select.");

        for (int i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &tmp_fds)) {
                if (i == 0) {
                    memset(buffer, 0, BUFLEN);
                    fgets(buffer, BUFLEN - 1, stdin);
                    if (strncmp(buffer, "exit", 4) == 0) {
                        exitFlag = true;
                        break;
                    }
                    send(sockfd, buffer, strlen(buffer), 0);
                    /*
                        Verificarea ca se primeste comanda diferit de exit.
                        Aici voi face subscribe/unsubscribe topic
                    */
                } else if (i == sockfd) {
                    memset(buffer, 0, BUFLEN);
                    // receive message froms server
                    bytesRecv = recv(sockfd, buffer, BUFLEN, 0);
                    AssertTrue(bytesRecv < 0,
                               "Error receiving data from the server.\n");
                    if (bytesRecv == 0) {
                        close(sockfd);
                        return 0;
                    }
                    while (bytesRecv){
                        cout << "Received from server: " << buffer;
                        bytesRecv = recv(sockfd, buffer, BUFLEN, 0);
                        AssertTrue(bytesRecv < 0,
                                "Error receiving data from the server.\n");
                    }
                }
            }
        }
    }
    close(sockfd);
    return 0;
}
