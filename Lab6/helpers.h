#ifndef _HELPERS_H
#define _HELPERS_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

/*
* Macro de verificare a erorilo
* Exemplu: 
* 		int fd = open (file_name , O_RDONLY);	
* 		DIE( fd == -1, "open failed");
*/

#define DIE(assertion, call_description)				\
	do {								\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",			\
					__FILE__, __LINE__);		\
			perror(call_description);			\
			exit(EXIT_FAILURE);				\
		}							\
	} while(0)
	
/* Dimensiunea maxima a calupului de date */
#define BUFLEN 1500	

#endif
