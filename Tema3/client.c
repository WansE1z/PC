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
    int sockfd;
    char *cookies[1];
    char *login[1];
    char *book[1];
    char *message;
    char *response;
    char task[50], title[BUFLEN], author[BUFLEN], username[BUFLEN],
        password[BUFLEN], token[BUFLEN];
    bool exitFlag = false, logged, entered_library = false;
    while (!exitFlag) {
        scanf("%s", task);
        if (strcmp(task, "register") == 0) {
            // open connection
            sockfd =
                open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);

            // print the prompts and read the username and password
            printf("username= ");
            scanf("%s", username);
            printf("password= ");
            scanf("%s", password);

            // put in the login token array the complete form including user and
            // pass
            JSON_Value *value = json_value_init_object();
            JSON_Object *object = json_value_get_object(value);
            json_object_set_string(object, "username", username);
            json_object_set_string(object, "password", password);
            login[0] = json_serialize_to_string_pretty(value);

            // compute the post request in order to get the message
            message = compute_post_request(
                "3.8.116.10", "/api/v1/tema/auth/register", "application/json",
                login, 1, NULL, 0, NULL);

            // send the message to the server
            send_to_server(sockfd, message);

            // get a response
            response = receive_from_server(sockfd);

            // ptr returns the string starting with the keyword searched
            char *ptr = strstr(response, "taken");

            /*
             if ptr contains anything, that means taken was found, which means
             the user is registered, so can't be registered once more
            */
            if (ptr != NULL) {
                printf("The username is taken, please choose another one.\n");
            } else {
                printf("You have successfully registered!\n");
            }

            // close the connection
            close_connection(sockfd);
        } else if (strcmp(task, "login") == 0) {
            // logged is a boolean variable that keeps track if the user is
            // logged or not
            if (!logged) {
                sockfd =
                    open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
                logged = true;
                char cookie[100];
                char *login[1];
                char username[BUFLEN];
                char password[BUFLEN];

                printf("username= ");
                scanf("%s", username);
                printf("password= ");
                scanf("%s", password);

                JSON_Value *value = json_value_init_object();
                JSON_Object *object = json_value_get_object(value);
                json_object_set_string(object, "username", username);
                json_object_set_string(object, "password", password);
                login[0] = json_serialize_to_string_pretty(value);

                message = compute_post_request(
                    "3.8.116.10", "/api/v1/tema/auth/login", "application/json",
                    login, 1, NULL, 0, NULL);
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
            } else {
                printf("You are already logged in.\n");
            }
        } else if (strcmp(task, "logout") == 0) {
            if (logged) {
                logged = false;
                sockfd =
                    open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
                message =
                    compute_get_request("3.8.116.10", "/api/v1/tema/auth/logout",
                                        NULL, cookies, 1, NULL);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);
                if (cookies != NULL) {
                    printf("Logged out.\n");
                }
                close_connection(sockfd);
            } else {
                printf(
                    "Can't log out a client that hasn't been logged in yet.\n");
            }
        } else if (strcmp(task, "enter_library") == 0) {
            if (logged) {
                entered_library = true;
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
        } else if (strcmp(task, "get_books") == 0) {
            if (logged && entered_library) {
                sockfd =
                    open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
                message = compute_get_request("3.8.116.10",
                                              "/api/v1/tema/library/books", NULL,
                                              cookies, 1, token);
                if (token != NULL) {
                    printf("Books were accessed.\n");
                } else {
                    printf("Error getting the books.\n");
                }
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);
                char *ptr = strstr(response, "[");
                printf("%s\n", ptr);
            } else {
                printf(
                    "Not logged or you didn't enter the library, no access.\n");
            }
            close_connection(sockfd);
        } else if (strcmp(task, "get_book") == 0) {
            if (logged && entered_library) {
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
                char *ptr = strstr(response, "[");
                printf("%s\n", ptr);
            } else {
                printf(
                    "Not logged or you didn't enter the library, no access.\n");
            }
            char *ptr = strstr(response, "title");
            char *ptr1 = strstr(response, "author");
            ptr += 7;
            strtok(ptr, ",");
            strcpy(title, ptr);

            ptr1 += 8;
            strtok(ptr, ",");
            strcpy(author, ptr1);
        } else if (strcmp(task, "add_book") == 0) {
            sockfd =
                open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
            char title2[BUFLEN];
            char author2[BUFLEN];
            char publisher[BUFLEN];
            char genre[BUFLEN];
            int page_count;

            printf("title=");
            scanf("%s", title2);

            printf("author=");
            scanf("%s", author2);

            printf("publisher=");
            scanf("%s", publisher);

            printf("genre=");
            scanf("%s", genre);
            
            printf("page_count=");
            scanf("%d", &page_count);

            JSON_Value *value2 = json_value_init_object();
            JSON_Object *object2 = json_value_get_object(value2);
            json_object_set_string(object2, "title", title2);
            json_object_set_string(object2, "author", author2);
            json_object_set_string(object2, "publisher", publisher);
            json_object_set_string(object2, "genre", genre);
            json_object_set_number(object2, "page_count", page_count);
            book[0] = json_serialize_to_string_pretty(value2);

            printf("THE TOKEN: %s\n", token);
            printf("THE MESSAGE: %s\n", book[0]);

            message = compute_post_request(
                "3.8.116.10", "/api/v1/tema/library/books", "application/json",
                book, 1, NULL, 0, token);
            send_to_server(sockfd, message);

            printf("%s\n", message);
            response = receive_from_server(sockfd);
            printf("%s", response);
            close_connection(sockfd);
        } else if (strcmp(task, "delete_book") == 0) {
            char read[BUFLEN];
            printf("id=");
            scanf("%s", read);

            char api[BUFLEN] = "/api/v1/tema/library/books/";
            strcat(api, read);

            sockfd =
                open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
            message =
                compute_delete_request("3.8.116.10", api, NULL, cookies, 1, token);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            printf("%s", response);
        }

        else if (strcmp(task, "exit") == 0) {
            exitFlag = true;
            printf("Client disconnected.\n");
        } else {
            printf("Please enter a valid command.\n");
        }
    }
}
