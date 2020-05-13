#ifndef _UTILS_
#define _UTILS_

#include <arpa/inet.h>
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <stdio.h>
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <unistd.h>     /* read, write, close */

#include "helpers.h"

#define REGISTER 1
#define LOGIN 2
#define ENTER_LIBRARY 3
#define GET_BOOKS 4
#define GET_BOOK 5
#define ADD_BOOK 6
#define DELETE_BOOK 7
#define LOGOUT 8
#define EXIT 9
#define NOTVALID 10

int clientSwitch(char task[]);

#endif