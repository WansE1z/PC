#include <bits/stdc++.h>
#include "include/helpers.h"
using namespace std;

int main(int argc, char *argv[]){
    int sfd,fdmax;
    int n,ver;
    char buffer[BUFLEN];
    fd_set read_fds;
    fd_set tmp_fds;
    FD_ZERO(&tmp_fds);
    FD_ZERO(&read_fds);

    if (argc < 3){
        exit(0);
    }

    sfd = socket(AF_INET, SOCK_STREAM,0);
    AssertTrue(sfd < 0, "socket has a problem");

    FD_SET(sfd, &read_fds);
    FD_SET(0, &read_fds);
    fdmax = sfd;

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));

    ver = inet_aton(argv[1], &serv_addr.sin_addr);
    AssertTrue(ver < 0, "Inet_aton has a problem");

    ver = connect(sfd, (sockaddr *)&serv_addr, sizeof(serv_addr));
    AssertTrue (ver < 0, "Connect could not have been made");

    while(1){
        tmp_fds = read_fds;
        ver = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
        AssertTrue (ver < 0, "Select has a problem");

        if (FD_ISSET(0, &tmp_fds)) {
            memset(buffer, 0, BUFLEN);
            fgets(buffer, BUFLEN - 1, stdin);
            if (!strcmp (buffer, "exit\n")){
                cout << "Client disconnected from server.";
                break;
            }
            n = send(sfd, buffer, BUFLEN, 0);
            AssertTrue(n < 0, "Send has a problem");
            printf("A client sent the message: %s\n", buffer);
        } 
        if (FD_ISSET(sfd, &tmp_fds)){
            memset(buffer,0,BUFLEN);
            n = recv(sfd,buffer,BUFLEN-1,0);
            AssertTrue(n < 0, "Error from server"); 
            if (n == 0){
                cout << "Server closed connection with the client.";
                break;
            }    
            cout << "Server sent the message " << buffer;
        }
    }
    close(sfd);
    shutdown(sfd,SHUT_RDWR);
}
