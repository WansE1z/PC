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
  int sockfd, page_count;
  char *cookies[1];
  char *login[1];
  char *book[1];
  char *message;
  char *response;
  char task[50], title[BUFLEN], author[BUFLEN], username[BUFLEN],
      password[BUFLEN], token[BUFLEN], cookie[100], publisher[BUFLEN],
      genre[BUFLEN];
  char ip[16] = "3.8.116.10";
  bool exitFlag = false, logged, entered_library = false, goodFormat = true;
  while (!exitFlag) {
    scanf("%s", task);
    if (strcmp(task, "register") == 0) {
      // open connection
      sockfd = open_connection(ip, 8080, AF_INET, SOCK_STREAM, 0);

      // print the prompts and read the username and password
      printf("username=");
      scanf("%s", username);
      printf("password=");
      scanf("%s", password);

      // put in the login token array the complete form including user and
      // pass
      JSON_Value *value = json_value_init_object();
      JSON_Object *object = json_value_get_object(value);
      json_object_set_string(object, "username", username);
      json_object_set_string(object, "password", password);
      login[0] = json_serialize_to_string_pretty(value);

      // compute the post request in order to get the message
      message =
          compute_post_request(ip, "/api/v1/tema/auth/register",
                               "application/json", login, 1, NULL, 0, NULL);

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
      /*
       logged is a boolean variable that keeps track if the user is
       logged or not
      */
      if (!logged) {
        sockfd = open_connection(ip, 8080, AF_INET, SOCK_STREAM, 0);
        logged = true;
        // update the status of the client

        printf("username=");
        scanf("%s", username);
        printf("password=");
        scanf("%s", password);
        // print the prompts as requested

        /*
            in the below sequence, i use parson in order to get a
            json value and object, that stores two string
           (username,password) in order to have an "application/json"
           payload
            {
                "username": String,
                "password": String
            }

        */
        JSON_Value *value = json_value_init_object();
        JSON_Object *object = json_value_get_object(value);
        json_object_set_string(object, "username", username);
        json_object_set_string(object, "password", password);
        login[0] = json_serialize_to_string_pretty(value);

        // compute the post message
        message =
            compute_post_request(ip, "/api/v1/tema/auth/login",
                                 "application/json", login, 1, NULL, 0, NULL);

        // send the details to the server
        send_to_server(sockfd, message);

        // get the response
        response = receive_from_server(sockfd);

        /*
         * ptr stores a string containing everything from "Set-Cookie" to
         * the end of that char array
         */
        char *ptr = strstr(response, "Set-Cookie: ");

        // if it is null, this means there was no cookie created, so it
        // is not valid
        if (ptr == NULL) {
          printf("Login failed.\n");
        }

        // go with the ptr until it reaches an ;, meaning that is the end
        // of the cookie
        strtok(ptr, ";");

        // add to the pointer 12, in order to get rid of "Set-Cookie:"
        ptr += 12;

        // store into an other array the cookie
        strcpy(cookie, ptr);

        // because of the fact that the compute_post_request needs an
        // char** i put the cookie in the char**
        cookies[0] = cookie;

        // if there exist a cookie, then the login is valid
        if (cookie != NULL) {
          printf("Login was made with success.\n");
        }

        // closing the connection
        close_connection(sockfd);
      } else {
        // if the logged boolean is true, that means an user is connected
        printf("You are already logged in.\n");
      }
    } else if (strcmp(task, "enter_library") == 0) {
      if (logged) {
        // a boolean that keeps track if the user has accesed the library
        entered_library = true;

        // open the connection
        sockfd = open_connection(ip, 8080, AF_INET, SOCK_STREAM, 0);

        // compute the message
        message = compute_get_request(ip, "/api/v1/tema/library/access", NULL,
                                      cookies, 1, NULL);

        // send details to the server
        send_to_server(sockfd, message);

        // get the response
        response = receive_from_server(sockfd);

        // ptr keeps track if the response that it gets contains token
        // if it does so, that means there was no token created
        char *ptr = strstr(response, "token");

        // if it is null, the token hasn't been created, so i do
        // not grant the acces to the library
        if (ptr == NULL) {
          printf("You do not have access to the library.\n");
        } else {
          // {"token" :}, have to add 8 to get to the token
          ptr += 8;
          memset(token, 0, BUFLEN);

          // move the token into the array
          strcpy(token, ptr);

          // cut the not necessary chars
          token[strlen(token) - 2] = '\0';

          // the token was created, so the access is permitted
          printf("You have gained access to the library.\n");
        }

        // close the connection
        close_connection(sockfd);
      } else {
        // if the logged is false, the user isn't connected
        printf("Not logged, no access.\n");
      }
    } else if (strcmp(task, "get_books") == 0) {
      // in order to get the books, the user has to be logged and
      // has to have access to the library
      if (logged && entered_library) {
        // open the connection
        sockfd = open_connection(ip, 8080, AF_INET, SOCK_STREAM, 0);

        // compute the message, giving to the request the cookie
        // and the token
        message = compute_get_request(ip, "/api/v1/tema/library/books", NULL,
                                      cookies, 1, token);

        // if there exist a token, that means the access was granted
        if (token != NULL) {
          printf("Books were accessed. ");
        } else {
          printf("Error getting the books.\n");
          break;
        }

        // send details to the server
        send_to_server(sockfd, message);

        // get the response
        response = receive_from_server(sockfd);

        // this pointer stores the list of books
        char *books = strstr(response, "[");
        printf("The books are:\n %s\n", books);

      } else {
        printf("Not logged or you didn't enter the library, no access.\n");
      }
      // close the connection
      close_connection(sockfd);
    } else if (strcmp(task, "get_book") == 0) {
      if (logged && entered_library) {
        // the api and the id that will be read
        char api[BUFLEN] = "/api/v1/tema/library/books/";
        char read[BUFLEN];

        // print the prompt, and read the id
        printf("id=");
        scanf("%s", read);

        // concat to the api the id
        strcat(api, read);

        // open the connection
        sockfd = open_connection(ip, 8080, AF_INET, SOCK_STREAM, 0);

        // compute the getting request, giving it
        message = compute_get_request(ip, api, NULL, cookies, 1, token);

        // sending the details to the server
        send_to_server(sockfd, message);

        // get the response from the server
        response = receive_from_server(sockfd);

        // if the pointer find that statement, that means the id is
        // incorrect, so print the message

        char *pointer = strstr(response, "No book was found!");
        if (pointer != NULL) {
          // the pointer found something, so print error
          printf(
              "The id you are trying to search for and get the "
              "information about doesn't exist.\n");

        } else {
          // return the list
          char *ptr = strstr(response, "[");
          printf("The details about the book are :\n %s\n", ptr);
        }
      } else {
        printf("Not logged or you didn't enter the library, no access.\n");
      }

    } else if (strcmp(task, "add_book") == 0) {
      // goodFormat keeps track if the format is good
      goodFormat = true;
      if (entered_library && logged) {
        // open the connection
        sockfd = open_connection(ip, 8080, AF_INET, SOCK_STREAM, 0);

        // print the prompts as requested
        // if any of the fields will be filled with " ", throw error
        printf("title=");
        scanf("%s", title);
        if (strcmp(title, " ") == 0) {
          goodFormat = false;
        }

        printf("author=");
        scanf("%s", author);
        if (strcmp(author, " ") == 0) {
          goodFormat = false;
        }

        printf("publisher=");
        scanf("%s", publisher);
        if (strcmp(publisher, " ") == 0) {
          goodFormat = false;
        }

        printf("genre=");
        scanf("%s", genre);
        if (strcmp(genre, " ") == 0) {
          goodFormat = false;
        }

        // if the page count is negative, throw error
        printf("page_count=");
        scanf("%d", &page_count);
        if (page_count < 0) {
          goodFormat = false;
        }

        // if the goodFormat is false, print error
        if (!goodFormat) {
          printf("The format isn't correct.\n");
          break;
        }

        // with the help of parson, i am using a json value and object
        // that stores the string and the number(page count)
        // that will make it look "pretty"
        // it will give the json in order to make an "application/json"
        /*
            {
                "title": String,
                "author": String,
                "genre": String,
                "page_count": Number
                "publisher": String
            }
        */

        JSON_Value *value2 = json_value_init_object();
        JSON_Object *object2 = json_value_get_object(value2);
        json_object_set_string(object2, "title", title);
        json_object_set_string(object2, "author", author);
        json_object_set_string(object2, "publisher", publisher);
        json_object_set_string(object2, "genre", genre);
        json_object_set_number(object2, "page_count", page_count);
        book[0] = json_serialize_to_string_pretty(value2);

        // compute the post request
        message =
            compute_post_request(ip, "/api/v1/tema/library/books",
                                 "application/json", book, 1, NULL, 0, token);

        // send details to the server
        send_to_server(sockfd, message);

        // get the response from the server
        response = receive_from_server(sockfd);

        // close the connection
        close_connection(sockfd);

        // if the goodFormat is true, then print the book
        printf("The book has been added!:\n %s\n", book[0]);
      } else {
        printf("Not logged or you didn't enter the library, no access.\n");
      }

    } else if (strcmp(task, "delete_book") == 0) {
      // if the user entered the library and also is logged in
      if (entered_library && logged) {
        // the api and the id that will be read
        char api[BUFLEN] = "/api/v1/tema/library/books/";
        char read[BUFLEN];

        // print the prompt, and read the id
        printf("id=");
        scanf("%s", read);

        // concat the id to the api
        strcat(api, read);

        // open the connection
        sockfd = open_connection(ip, 8080, AF_INET, SOCK_STREAM, 0);

        // compute the delete request
        message = compute_delete_request(ip, api, NULL, cookies, 1, token);

        // send the details to the server
        send_to_server(sockfd, message);

        // get the response from the server
        response = receive_from_server(sockfd);

        // if the pointer finds that statement, then the id isn't in the
        // books list
        char *ptr = strstr(response, "No book was deleted!");
        if (ptr != NULL) {
          // if it found the statement, then print error
          printf(
              "The id you are trying to delete doesn't exist in the "
              "list of books.\n");
        } else {
          printf("The book with id %s got deleted.\n", read);
        }
      } else {
        printf("Not logged or you didn't enter the library, no access.\n");
      }
    } else if (strcmp(task, "logout") == 0) {
      if (logged) {
        // if the user is logged, the user can be logged out
        logged = false;

        // open the connection
        sockfd = open_connection(ip, 8080, AF_INET, SOCK_STREAM, 0);

        // compute the message with a get request
        message = compute_get_request(ip, "/api/v1/tema/auth/logout", NULL,
                                      cookies, 1, NULL);

        // send details to the server
        send_to_server(sockfd, message);

        // get the response from the server
        response = receive_from_server(sockfd);

        // if there exist a cookie linked to the user and the user is
        // logged in then print the message
        if (cookies != NULL) {
          printf("The client logged out.\n");
        }

        // close the connection
        close_connection(sockfd);
      } else {
        printf("Can't log out a client that hasn't been logged in yet.\n");
      }

    } else if (strcmp(task, "exit") == 0) {
      // if the exit command was received, break from the while
      exitFlag = true;
      printf("Client disconnected.\n");

    } else {
      // if none of the above commands were received, then the command is
      // incorrect
      printf("Please enter a valid command.\n");
    }
  }
}
