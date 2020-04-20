#ifndef _HELPERS_H
#define _HELPERS_H 1

using namespace std;
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <bits/stdc++.h>

#define AssertTrue(assertion, call_description)	\
	do {									\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",	\
					__FILE__, __LINE__);	\
			perror(call_description);		\
			exit(EXIT_FAILURE);				\
		}									\
	} while(0)

#define BUFLEN		1560	// dimensiunea maxima a calupului de date
#define MAX_CLIENTS	10000	// numarul maxim de clienti in asteptare
#define MAX_RECV (sizeof(cli_msg) + 1)
#endif
