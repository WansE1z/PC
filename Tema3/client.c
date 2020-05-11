#include <arpa/inet.h>
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <stdbool.h>
#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <unistd.h>     /* read, write, close */

#include "helpers.h"
#include "parson.h"
#include "requests.h"

int main(int argc, char *argv[]) {
    char token[BUFLEN];
    char *cookies[1];
    char *message;
    char *response;
    int sockfd;
    char task[50];
    bool exitFlag = false;
    bool taskFlag;
    bool logged;
    while (!exitFlag) {
        ;
        taskFlag = false;
        scanf("%s", task);
        if (strcmp(task, "register") == 0) {
            taskFlag = true;
            char *login[1];
            sockfd =
                open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
            char buffer[10000];
            char username[BUFLEN];
            char password[BUFLEN];

            printf("username= ");
            scanf("%s", username);
            printf("password= ");
            scanf("%s", password);

            memset(buffer, 0, BUFLEN);
            sprintf(buffer, "username=%s&password=%s&", username, password);
            login[0] = buffer;

            message = compute_post_request(
                "3.8.116.10", "/api/v1/tema/auth/register",
                "application/x-www-form-urlencoded", login, 1, NULL, 0);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            char *ptr = strstr(response, "taken");
            if (ptr != NULL) {
                printf("The username is taken, please choose another one.\n");
                break;
            } else {
                printf("You have successfully registered!\n");
            }
            close_connection(sockfd);
        }
        if (strcmp(task, "login") == 0) {
            logged = true;
            taskFlag = true;
            sockfd =
                open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
            char cookie[100];
            char *login[1];
            char buffer[10000];
            char username[BUFLEN];
            char password[BUFLEN];

            printf("username= ");
            scanf("%s", username);
            printf("password= ");
            scanf("%s", password);
            memset(buffer, 0, BUFLEN);
            sprintf(buffer, "username=%s&password=%s&", username, password);
            login[0] = buffer;

            message = compute_post_request(
                "3.8.116.10", "/api/v1/tema/auth/login",
                "application/x-www-form-urlencoded", login, 1, NULL, 0);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            char *ptr = strstr(response, "Set-Cookie: ");
            if (ptr == NULL) {
                printf("Login failed.\n");
                break;
            }
            strtok(ptr, ";");
            ptr += 12;
            strcpy(cookie, ptr);
            cookies[0] = cookie;
            if (cookie != NULL) {
                printf("Login was made with success.\n");
            }
            close_connection(sockfd);
        }
        if (strcmp(task, "logout") == 0) {
            logged = false;
            taskFlag = true;
            sockfd =
                open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
            message =
                compute_get_request("3.8.116.10", "/api/v1/tema/auth/logout",
                                    NULL, cookies, 1, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            if (cookies != NULL) {
                printf("Logged out.\n");
            } else {
                printf("The cookie is not valid, can't log out.");
            }
            close_connection(sockfd);
        }
        if (strcmp(task, "enter_library") == 0) {
            taskFlag = true;
            if (logged) {
                sockfd =
                    open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
                message = compute_get_request("3.8.116.10",
                                              "/api/v1/tema/library/access",
                                              NULL, cookies, 1, NULL);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);
                char *ptr = strstr(response, "token");
                if (ptr == NULL) {
                    printf("You do not have access to the library.\n");
                } else {
                    ptr += 8;
                    memset(token, 0, BUFLEN);
                    strcpy(token, ptr);
                    token[strlen(token) - 2] = '\0';
                    printf("You have gained access to the library.\n");
                }
                close_connection(sockfd);
            } else {
                printf("Not logged, no access.\n");
            }
        }
        if (strcmp(task, "get_books") == 0) {
            taskFlag = true;
            if (logged) {
                sockfd =
                    open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
                message = compute_get_request("3.8.116.10",
                                              "/api/v1/tema/library/books", NULL,
                                              cookies, 1, token);
                printf("Token is : %s", token);
                if (token != NULL) {
                    printf("Books were accessed.\n");
                } else {
                    printf("Error getting the books.\n");
                }
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);
                char *ptr = strstr(response, "[");
                printf("%s", ptr);
            } else {
                printf("Not logged, no access.\n");
            }
            /*
                TODO: afisare get books + get book
            */
            // char *ptr = strstr(response,"id");
            // i = 0;
            // while (ptr != NULL){
            //     ptr += 4;
            //     strtok(ptr,",");
            //     strcpy(id[i],ptr);

            //     ptr += 12;
            //     strtok(ptr,"}");
            //     strcpy(title[i],ptr);
            //     title[strlen(title)-1][] = '\0';
            //     strstr(response,"id");
            // }
            close_connection(sockfd);
        }
        if (strcmp(task, "get_book") == 0) {
            taskFlag = true;
            if (logged) {
                char read[BUFLEN];
                printf("id=");
                scanf("%s", read);

                char api[BUFLEN] = "/api/v1/tema/library/books/";
                strcat(api, read);

                sockfd =
                    open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
                message = compute_get_request("3.8.116.10", api, NULL, cookies,
                                              1, token);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);
            } else {
                printf("Not logged, no access.\n");
            }
            // char* ptr = strstr(response,"title");
            // ptr += 7;
            // strtok(ptr,",");
            // strcpy(title,ptr);
            // printf("%s\n",title);
        }
        if (strcmp(task, "exit") == 0) {
            taskFlag = exitFlag = true;
            printf("Client disconnected.\n");
        }
        if (!taskFlag) {
            printf("Please enter a valid command.\n");
        }
    }
}
