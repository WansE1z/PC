/*
 * Protocoale de comunicatii
 * Laborator 7 - TCP
 * Echo Server
 * server.c
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#include "helpers.h"

// Primeste date de pe connfd1 si trimite mesajul receptionat pe connfd2, la serverul de echo connfd1 == connfd2
int receive_and_send(int connfd1, int connfd2)
{
    char buf[BUFLEN];
    int bytes_received = 0;

    // TODO 6: Receptionati un mesaj de la connfd1
    bytes_received = recv(connfd1,buf,BUFLEN,0);

    if (bytes_received != 0) {
        fprintf(stderr, "Received: %s", buf);
    }

    send(connfd2,buf,bytes_received,0);
    // TODO 7: Timiteti mesajul la connfd2

    return bytes_received;
}

void run_echo_server(int listenfd)
{
    struct sockaddr_in client_addr;

    int bytes_received;
    int connfd = -1;
    socklen_t socket_len = sizeof(struct sockaddr_in);

    // TODO 4: Ascultati pentru un singur client pe socketul dat
    DIE(listen(listenfd, 1) == -1, "Nope.");
    // TODO 5: Acceptati o conexiune
    connfd = accept(listenfd,(struct sockaddr*)&client_addr,&socket_len);
    do {
        bytes_received = receive_and_send(connfd, connfd);
    } while (bytes_received > 0);

    // TODO 8: Inchideti conexiunea si socket-ul clientului
    close(connfd);
    shutdown(connfd, SHUT_RDWR);
}

void run_chat_server(int listenfd)
{
    struct sockaddr_in client_addr1;
    struct sockaddr_in client_addr2;
    int bytes_received;
    int connfd1 = -1;
    int connfd2 = -1;
    int listenfd1 = -1;
    int listenfd2 = -1;
    socklen_t socket_len = sizeof(struct sockaddr_in);

    // TODO 4: Ascultati pentru doi clineti pe socketul dat
    listenfd1 = listen(listenfd,3);
    listenfd2 = listen(listenfd,3);
    // TODO 5: Acceptati doua conexiuni
    connfd1 = accept(listenfd1,(struct sockaddr*)&client_addr1,&socket_len);
    connfd2 = accept(listenfd2,(struct sockaddr*)&client_addr2,&socket_len);

    do {
        bytes_received = receive_and_send(connfd1, connfd2);

        if (bytes_received == 0)
            break;

        bytes_received = receive_and_send(connfd2, connfd1);
    } while (bytes_received > 0);

    close(connfd1);
    close(connfd2);
    close(listenfd);
    shutdown(connfd2,SHUT_RDWR);
    shutdown(connfd1,SHUT_RDWR);
    shutdown(listenfd,SHUT_RDWR);
    // TODO 8: Inchideti conexiunile si socketii creati
}

int main(int argc, char* argv[])
{
    int sockfd = -1;
    socklen_t socket_len = sizeof(struct sockaddr_in);


    if (argc != 3) {
        printf("\n Usage: %s <ip> <port>\n", argv[0]);
        return 1;
    }


    // TODO 1: Creati un socket TCP pentru receptionarea conexiunilor
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    // TODO 2: Completati in serv_addr adresa serverului pentru bind, familia de adrese si portul rezervat pentru server
    struct sockaddr_in in;
    in.sin_family = AF_INET;
    in.sin_port = htons(atoi(argv[2]));
    in.sin_addr.s_addr = INADDR_ANY;
    // TODO 3: Asociati adresa serverului cu socketul creat folosind bind
    DIE(bind(sockfd, (struct sockaddr*)&in,socket_len)<0, "Can not bind.");
    run_echo_server(sockfd);
    run_chat_server(sockfd);
    // TODO 9: Inchideti socketul creat
    close(sockfd);

    return 0;
}
