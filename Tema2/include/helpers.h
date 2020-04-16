#include "serv_utils.h"

#ifndef _HELPERS_H
#define _HELPERS_H 1

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define AssertTrue(assertion, call_description)	\
	do {									\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",	\
					__FILE__, __LINE__);	\
			perror(call_description);		\
			exit(EXIT_FAILURE);				\
		}									\
	} while(0)

#define BUFLEN		256	// dimensiunea maxima a calupului de date
#define MAX_CLIENTS	10000	// numarul maxim de clienti in asteptare
#define MAX_RECV (sizeof(cli_msg) + 1)
#endif
